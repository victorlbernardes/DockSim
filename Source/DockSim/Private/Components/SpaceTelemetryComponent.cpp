#include "Components/SpaceTelemetryComponent.h"
#include "Components/StaticMeshComponent.h"
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

	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceDockingPort::StaticClass());
	ASpaceDockingPort* TargetPort = Cast<ASpaceDockingPort>(FoundActor);

	if (TargetPort && TargetPort->DockingAnchor)
	{
		FVector ShipLoc = GetOwner()->GetActorLocation();
		FVector PortLoc = TargetPort->DockingAnchor->GetComponentLocation();
		FVector PortForward = TargetPort->DockingAnchor->GetForwardVector();
		FVector ShipForward = GetOwner()->GetActorForwardVector();

		// 1. DISTÂNCIA ESCALAR (Unidade: Metros)
		float DistanceMeters = FVector::Dist(ShipLoc, PortLoc) / 100.0f;

		// 2. ALINHAMENTO DE ROTAÇÃO (Pitch/Yaw)
		// Verifica se o nariz da nave está paralelo ao eixo do porto
		// Usamos -PortForward porque a nave deve olhar CONTRA o porto para acoplar
		float RotDot = FVector::DotProduct(ShipForward, -PortForward);
		float RotPercent = FMath::Clamp(RotDot * 100.0f, 0.0f, 100.0f);

		// 3. ALINHAMENTO DE TRANSLAÇÃO (O "Corredor")
		// Criamos um vetor que vai do porto até à nave e normalizamos
		FVector DirToShip = (ShipLoc - PortLoc).GetSafeNormal();
		
		// Comparamos: A nave está no "trilho" que sai do porto?
		// Se a nave estiver exatamente na frente, DirToShip será igual ao PortForward (Dot = 1.0)
		float PathDot = FVector::DotProduct(PortForward, DirToShip);
		float PathPercent = FMath::Clamp(PathDot * 100.0f, 0.0f, 100.0f);

		// 4. PRECISÃO TOTAL (Cálculo Punitivo)
		// Multiplicamos os dois. Se um estiver em 0%, o total é 0%. 
		// Isso obriga o jogador a ser perfeito em ambos.
		float TotalAlignment = (RotPercent * PathPercent) / 100.0f;

		// 5. FEEDBACK VISUAL (Logs)
		if (GEngine)
		{
			FColor DisplayColor = (TotalAlignment > 95.0f) ? FColor::Cyan : FColor::Yellow;
			
			FString DebugMsg = FString::Printf(TEXT("DIST: %.2fm | ROTAÇÃO: %.1f%% | CORREDOR: %.1f%% | TOTAL: %.1f%%"), 
				DistanceMeters, RotPercent, PathPercent, TotalAlignment);
			
			GEngine->AddOnScreenDebugMessage(2, 0.0f, DisplayColor, DebugMsg);
		}
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
		Data.CurrentEngineScale = MovementComp->GetMasterPowerScale();
	}
	return Data;
}

bool USpaceTelemetryComponent::IsReadyForHardLock() const
{
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceDockingPort::StaticClass());
	ASpaceDockingPort* TargetPort = Cast<ASpaceDockingPort>(FoundActor);

	if (!TargetPort || !TargetPort->DockingAnchor) return false;

	FVector ShipLoc = GetOwner()->GetActorLocation();
	FVector PortLoc = TargetPort->DockingAnchor->GetComponentLocation();
	FVector PortForward = TargetPort->DockingAnchor->GetForwardVector();
	FVector ShipForward = GetOwner()->GetActorForwardVector();

	// Recalcula os valores cruciais no exato milissegundo do aperto do botão
	float DistanceMeters = FVector::Dist(ShipLoc, PortLoc) / 100.0f;
	float RotDot = FVector::DotProduct(ShipForward, -PortForward);
	float RotPercent = FMath::Clamp(RotDot * 100.0f, 0.0f, 100.0f);
	FVector DirToShip = (ShipLoc - PortLoc).GetSafeNormal();
	float PathDot = FVector::DotProduct(PortForward, DirToShip);
	float PathPercent = FMath::Clamp(PathDot * 100.0f, 0.0f, 100.0f);
	float TotalAlignment = (RotPercent * PathPercent) / 100.0f;

	// A REGRA DE OURO DO SEU JOGO (Ajuste a dificuldade aqui)
	// Exemplo: Menos de 2.5 metros e Mais de 95% de alinhamento
	if (DistanceMeters <= 10.0f && TotalAlignment >= 95.0f)
	{
		return true;
	}

	return false;
}