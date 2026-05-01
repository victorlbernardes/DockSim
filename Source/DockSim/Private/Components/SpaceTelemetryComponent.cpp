#include "Components/SpaceTelemetryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DockingPort/SpaceDockingPort.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SpaceShipPawn.h"

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
       // --- BUSCA O PONTO DE DOCKING ESPECÍFICO ---
       FVector ShipLoc;
       FVector ShipForward;

       // Tentamos encontrar o DockingPoint definido na nave (ASpaceShipPawn)
       if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(GetOwner()))
       {
          // Usamos o componente que você criou (onde está a câmera)
          USceneComponent* DPoint = Ship->GetDockingPoint(); 
          ShipLoc = DPoint->GetComponentLocation();
          ShipForward = DPoint->GetForwardVector();
       }
       else
       {
          // Fallback caso não seja a nave do jogador
          ShipLoc = GetOwner()->GetActorLocation();
          ShipForward = GetOwner()->GetActorForwardVector();
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
		// 1. Pega a velocidade global da SUA nave
		FVector ShipGlobalVelocity = RootMesh->GetPhysicsLinearVelocity();
		FVector TargetGlobalVelocity = FVector::ZeroVector;

		// 2. Descobre a velocidade global do ALVO (A Estação)
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceDockingPort::StaticClass());
		if (ASpaceDockingPort* TargetPort = Cast<ASpaceDockingPort>(FoundActor))
        		{
			if (UPrimitiveComponent* PortRoot = Cast<UPrimitiveComponent>(TargetPort->GetRootComponent()))
			{
				TargetGlobalVelocity = PortRoot->GetPhysicsLinearVelocity();
			}

			USceneComponent* DPoint = nullptr;
			if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(Owner)) 
			{ 
				DPoint = Ship->GetDockingPoint(); 
			}
            
			FVector ShipLoc = DPoint ? DPoint->GetComponentLocation() : Owner->GetActorLocation();
			FVector ShipForward = DPoint ? DPoint->GetForwardVector() : Owner->GetActorForwardVector();
			// NOVO: Pegando o "Teto" da Nave
			FVector ShipUp = DPoint ? DPoint->GetUpVector() : Owner->GetActorUpVector();

			FVector PortLoc = TargetPort->DockingAnchor->GetComponentLocation();
			FVector PortForward = TargetPort->DockingAnchor->GetForwardVector();
			// NOVO: Pegando o "Teto" da Estação
			FVector PortUp = TargetPort->DockingAnchor->GetUpVector();

			Data.Distance = FVector::Dist(ShipLoc, PortLoc) / 100.0f;

			// 1. Alinhamento de Bico (Frente)
			float RotDot = FVector::DotProduct(ShipForward, -PortForward);
			float RotPercent = FMath::Clamp(RotDot * 100.0f, 0.0f, 100.0f);

			// 2. Alinhamento de Caminho (Cone de Aproximação)
			FVector DirToShip = (ShipLoc - PortLoc).GetSafeNormal();
			float PathDot = FVector::DotProduct(PortForward, DirToShip);
			float PathPercent = FMath::Clamp(PathDot * 100.0f, 0.0f, 100.0f);
            
			// 3. NOVO: Alinhamento de Teto (Roll Match)
			float UpDot = FVector::DotProduct(ShipUp, PortUp);
			// Valores negativos viram 0% (se estiver de cabeça para baixo, não atraca!)
			Data.RollAlignmentPercent = FMath::Clamp(UpDot * 100.0f, 0.0f, 100.0f);

			// A MÁGICA: O Alinhamento Total agora multiplica TUDO. 
			// Se o Roll estiver 0%, o Total cai pra 0%.
			float AlignmentTemp = (RotPercent * PathPercent) / 100.0f;
			// SALVAMOS O BICO SEPARADO AQUI:
			Data.NoseAlignmentPercent = AlignmentTemp; 
        }

		// 3. A MÁGICA DA FÍSICA RELATIVA: Velocidade da Nave MENOS Velocidade do Alvo
		FVector RelativeVelocity = ShipGlobalVelocity - TargetGlobalVelocity;
        
		// 4. Transformamos a velocidade RELATIVA em Local (O Zero Psicológico continua aqui)
		// Agora, VelForward diz exatamente o quão rápido o BICO da nave está se aproximando do alvo!
		Data.VelForward = ApplyPsychologicalZero(FVector::DotProduct(RelativeVelocity, Owner->GetActorForwardVector()) / 100.f);
		Data.VelRight   = ApplyPsychologicalZero(FVector::DotProduct(RelativeVelocity, Owner->GetActorRightVector()) / 100.f);
		Data.VelUp      = ApplyPsychologicalZero(FVector::DotProduct(RelativeVelocity, Owner->GetActorUpVector()) / 100.f);
		Data.VelTotal = RelativeVelocity.Size() / 100.0f;
		
		// Rotações
		FRotator CurrentRot = Owner->GetActorRotation();
		Data.Pitch = CurrentRot.Pitch;
		Data.Yaw   = CurrentRot.Yaw;
		Data.Roll  = CurrentRot.Roll;
	}

	// Transfere o estado do Enum de forma limpa e direta
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

	// Usa as lógicas que definimos acima para garantir que TUDO será avaliado no juízo final
	FVector ShipLoc = GetOwner()->GetActorLocation();
	FVector ShipForward = GetOwner()->GetActorForwardVector();
	FVector ShipUp = GetOwner()->GetActorUpVector();

	FVector PortLoc = TargetPort->DockingAnchor->GetComponentLocation();
	FVector PortForward = TargetPort->DockingAnchor->GetForwardVector();
	FVector PortUp = TargetPort->DockingAnchor->GetUpVector();

	float DistanceMeters = FVector::Dist(ShipLoc, PortLoc) / 100.0f;

	float RotDot = FVector::DotProduct(ShipForward, -PortForward);
	float RotPercent = FMath::Clamp(RotDot * 100.0f, 0.0f, 100.0f);

	FVector DirToShip = (ShipLoc - PortLoc).GetSafeNormal();
	float PathDot = FVector::DotProduct(PortForward, DirToShip);
	float PathPercent = FMath::Clamp(PathDot * 100.0f, 0.0f, 100.0f);

	// Checa o Roll na hora do juízo final
	float UpDot = FVector::DotProduct(ShipUp, PortUp);
	float RollPercent = FMath::Clamp(UpDot * 100.0f, 0.0f, 100.0f);
	float NosePercent = (RotPercent * PathPercent) / 100.0f;

	// REGRA JUSTA SEPARADA
	if (DistanceMeters <= 10.0f && NosePercent >= 95.0f && RollPercent >= 95.0f)
	{
		return true;
	}

	return false;
}