#include "Components/SpaceTelemetryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpaceResourceComponent.h"
#include "Engine/Engine.h"

USpaceTelemetryComponent::USpaceTelemetryComponent()
{
	// A telemetria DEVE rodar todo frame para manter o painel da nave atualizado!
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USpaceTelemetryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Encontra o corpo físico da nave para poder medir a velocidade dele
	TargetMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
}

void USpaceTelemetryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

float USpaceTelemetryComponent::ApplyPsychologicalZero(float Value) 
{
	const float Threshold = 0.05f; // 5 cm/s
	return (FMath::Abs(Value) < Threshold) ? 0.0f : Value;
}

FSpaceTelemetryData USpaceTelemetryComponent::GetLatestTelemetry()
{
	FSpaceTelemetryData Data;
	AActor* Owner = GetOwner();
	UPrimitiveComponent* RootMesh = Cast<UPrimitiveComponent>(Owner->GetRootComponent());

	// 1. Encontra o Músculo da nave para ler o estado do motor
	USpaceMovementComponent* MovementComp = Owner->FindComponentByClass<USpaceMovementComponent>();
	USpaceResourceComponent* ResourceComp = Owner->FindComponentByClass<USpaceResourceComponent>();

	if (RootMesh)
	{
		FVector GlobalVelocity = RootMesh->GetPhysicsLinearVelocity();
        
		// Transformamos velocidade Global em Local (O Zero Psicológico continua aqui)
		Data.VelForward = ApplyPsychologicalZero(FVector::DotProduct(GlobalVelocity, Owner->GetActorForwardVector()) / 100.f);
		Data.VelRight   = ApplyPsychologicalZero(FVector::DotProduct(GlobalVelocity, Owner->GetActorRightVector()) / 100.f);
		Data.VelUp      = ApplyPsychologicalZero(FVector::DotProduct(GlobalVelocity, Owner->GetActorUpVector()) / 100.f);

		// Rotações
		FRotator CurrentRot = Owner->GetActorRotation();
		Data.Pitch = CurrentRot.Pitch;
		Data.Yaw   = CurrentRot.Yaw;
		Data.Roll  = CurrentRot.Roll;
	}

	// 2. Transfere o estado do Enum de forma limpa e direta
	if (MovementComp)
	{
		Data.ThrustMode = MovementComp->GetCurrentPowerMode();
	}

	// 3. Combustível
	if (ResourceComp)
	{
		Data.FuelPercent = ResourceComp->GetFuelPercent();
	}
	else
	{
		Data.FuelPercent = 0.0f; // Falha segura caso a nave não tenha tanque
	}
	return Data;
}