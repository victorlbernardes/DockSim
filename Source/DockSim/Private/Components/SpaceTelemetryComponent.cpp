#include "Components/SpaceTelemetryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpaceResourceComponent.h"
#include "DockingPort/SpaceDockingPort.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

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
	// 1. Busca a Estação no mapa (Excelente para testes/MVP)
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceDockingPort::StaticClass());
	ASpaceDockingPort* TargetPort = Cast<ASpaceDockingPort>(FoundActor);

	if (TargetPort && TargetPort->DockingAnchor)
	{
		// 2. Calcula a Distância (Convertendo unidades da Unreal de cm para metros)
		FVector ShipLoc = GetOwner()->GetActorLocation();
		FVector PortLoc = TargetPort->DockingAnchor->GetComponentLocation();
		float DistanceMeters = FVector::Dist(ShipLoc, PortLoc) / 100.0f;

		// 3. Calcula o Ângulo/Alinhamento (Dot Product)
		FVector ShipForward = GetOwner()->GetActorForwardVector();
		// Queremos que a nave olhe exatamente na direção oposta da face do porto
		FVector TargetFacing = -TargetPort->DockingAnchor->GetForwardVector(); 
		
		float AlignmentDot = FVector::DotProduct(ShipForward, TargetFacing);
		// Converte o Dot Product (-1 a 1) em Porcentagem (0% a 100%)
		float AlignmentPercent = FMath::Clamp(AlignmentDot * 100.0f, 0.0f, 100.0f);

		// 4. Exibe os Logs na Tela e no Output Log
		if (GEngine)
		{
			FString DebugMsg = FString::Printf(TEXT("ALVO: %.2fm | ALINHAMENTO: %.1f%%"), DistanceMeters, AlignmentPercent);
			
			// Usamos a 'Key 2' e tempo '0.0f' para a mensagem atualizar no mesmo lugar em vez de flodar a tela
			GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Yellow, DebugMsg);
		}
		
		// Descomente a linha abaixo se quiser que flode a janela de Output Log também
		// UE_LOG(LogTemp, Log, TEXT("DOCKING: Dist: %.2fm | Alinhamento: %.1f%%"), DistanceMeters, AlignmentPercent);
	}
	
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

	USpaceMovementComponent* MovementComp = Owner->FindComponentByClass<USpaceMovementComponent>();
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
	return Data;
}