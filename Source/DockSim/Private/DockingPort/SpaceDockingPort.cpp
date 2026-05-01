#include "DockingPort/SpaceDockingPort.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Player/SpaceShipPawn.h" 

ASpaceDockingPort::ASpaceDockingPort()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));

	DockStructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DockStructureMesh"));
	DockStructureMesh->SetupAttachment(RootComponent);

	DockingAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("DockingAnchor"));
	DockingAnchor->SetupAttachment(DockStructureMesh);

	// 2. O Anel Visual (Filho da âncora)
	DockVisualRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DockVisualRing"));
	DockVisualRing->SetupAttachment(DockingAnchor);
	DockVisualRing->SetCollisionProfileName(TEXT("NoCollision"));

	// Colocamos o modelo e giramos APENAS a arte! O Anchor pai continua reto.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TargetMesh(TEXT("/Engine/BasicShapes/Cylinder"));
	if (TargetMesh.Succeeded())
	{
		DockVisualRing->SetStaticMesh(TargetMesh.Object);
		DockVisualRing->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.1f));
		DockVisualRing->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f)); // A arte gira 90 graus. A matemática não.
	}

	// Lâmpada física criada no C++
	SirenBulb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SirenBulb"));
	SirenBulb->SetupAttachment(DockingAnchor);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMesh.Succeeded())
	{
		SirenBulb->SetStaticMesh(SphereMesh.Object);
		SirenBulb->SetRelativeScale3D(FVector(0.15f));
	}
	SirenBulb->SetCollisionProfileName(TEXT("NoCollision"));

	// Luz técnica
	BeaconLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BeaconLight"));
	BeaconLight->SetupAttachment(SirenBulb); 
	BeaconLight->SetMobility(EComponentMobility::Movable);
	BeaconLight->SetAttenuationRadius(2500.0f); 
	BeaconLight->bUseInverseSquaredFalloff = false; 

	// Área de Captura
	CaptureArea = CreateDefaultSubobject<UBoxComponent>(TEXT("CaptureArea"));
	CaptureArea->SetupAttachment(DockStructureMesh);
	CaptureArea->SetBoxExtent(FVector(1500.0f)); 
	CaptureArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CaptureArea->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	PrecisionDockingVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("PrecisionDockingVolume"));
	PrecisionDockingVolume->SetupAttachment(DockingAnchor);
    
	PrecisionDockingVolume->SetBoxExtent(FVector(4.0f, 50.0f, 50.0f)); 
	PrecisionDockingVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PrecisionDockingVolume->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshLoader(TEXT("/Engine/BasicShapes/Sphere"));

	// Vamos criar as luzes procedurais em um círculo perfeito
	for (int32 i = 0; i < NumberOfLights; i++)
	{
		// 1. Precisamos de um nome único para cada componente, senão a Unreal dá Crash!
		FName LightName = FName(*FString::Printf(TEXT("RunwayLight_%d"), i));
		
		// 2. Instancia o componente
		UStaticMeshComponent* LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(LightName);
		LightMesh->SetupAttachment(DockingAnchor);
		
		// 3. Configurações visuais e de colisão
		if (SphereMeshLoader.Succeeded())
		{
			LightMesh->SetStaticMesh(SphereMeshLoader.Object);
		}
		LightMesh->SetRelativeScale3D(FVector(0.1f)); // Bem pequenininhas
		LightMesh->SetCollisionProfileName(TEXT("NoCollision")); // Luz não tem física
		
		// 4. A MATEMÁTICA: Distribuir em um círculo nos eixos Y e Z
		// 360 graus divididos pelo número de luzes, convertido para radianos
		float AngleRads = (i * (360.0f / NumberOfLights)) * (PI / 180.0f);
		
		float YPos = FMath::Cos(AngleRads) * RunwayRingRadius;
		float ZPos = FMath::Sin(AngleRads) * RunwayRingRadius;
		
		LightMesh->SetRelativeLocation(FVector(0.0f, YPos, ZPos));

		// 5. Salva na nossa lista para podermos acessar depois se precisarmos
		RunwayLights.Add(LightMesh);
	}
}

void ASpaceDockingPort::BeginPlay()
{
	Super::BeginPlay();

	CaptureArea->OnComponentBeginOverlap.AddDynamic(this, &ASpaceDockingPort::OnCaptureAreaBeginOverlap);
	CaptureArea->OnComponentEndOverlap.AddDynamic(this, &ASpaceDockingPort::OnCaptureAreaEndOverlap);
	
	if (SirenBulb->GetMaterial(0))
	{
		DynamicSirenMaterial = SirenBulb->CreateDynamicMaterialInstance(0, SirenBulb->GetMaterial(0));
	}

	GetWorld()->GetTimerManager().SetTimer(SirenTimerHandle, this, &ASpaceDockingPort::UpdateSirenLogic, 0.05f, true);
	PrecisionDockingVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpaceDockingPort::OnPrecisionVolumeOverlap);
}

void ASpaceDockingPort::SetDockingStatus(EDockingStatus NewStatus)
{
	CurrentDockingStatus = NewStatus;
}

void ASpaceDockingPort::UpdateSirenLogic()
{
	if (!BeaconLight || !DynamicSirenMaterial) return;

	FLinearColor Color = FLinearColor::Black;
	float Alpha = 0.0f;
	float Time = GetWorld()->GetTimeSeconds();

	switch (CurrentDockingStatus)
	{
		case EDockingStatus::None:
			// AZUL CIANO - Pulso Suave (Seno) para Beacon de longa distância
			Color = FLinearColor(0.0f, 0.5f, 1.0f); 
			Alpha = (FMath::Sin(Time * 2.0f) + 1.0f) * 0.5f; // Pulsa suavemente
			break;

		case EDockingStatus::Standby:
			// LARANJA - Pisca Rítmico (Quadrado) para alerta de zona
			Color = FLinearColor(1.0f, 1.0f, 0.0f);
			Alpha = (FMath::Fmod(Time, 0.8f) < 0.4f) ? 1.0f : 0.0f; 
			break;

		case EDockingStatus::Granted:
			// VERDE - Sólido para sucesso
			Color = FLinearColor(0.0f, 1.0f, 0.0f);
			Alpha = 1.0f; 
			break;

		case EDockingStatus::Denied:
			// VERMELHO - Pisca Rápido para perigo
			Color = FLinearColor(1.0f, 0.0f, 0.0f);
			Alpha = (FMath::Fmod(Time, 0.2f) < 0.1f) ? 1.0f : 0.0f;
			break;
	}

	BeaconLight->SetLightColor(Color);
	BeaconLight->SetIntensity(Alpha * 60000.0f);
	
	DynamicSirenMaterial->SetVectorParameterValue(TEXT("BaseColor"), Color);
	DynamicSirenMaterial->SetScalarParameterValue(TEXT("EmissiveMult"), Alpha * 50.0f);
}

void ASpaceDockingPort::OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(OtherActor))
	{
		// 1. Acorda a Estação (Muda a luz para Laranja)
		SetDockingStatus(EDockingStatus::Standby);
		
		// 2. Avisa a Nave para ligar o HUD (Faz o TXT_AlertSitcher aparecer!)
		Ship->UpdateDockingSignal(ESignalStatus::Acquired);
	}
}

void ASpaceDockingPort::OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(OtherActor))
	{
		// 1. Apaga a Estação (Muda a luz para Ciano Pulsante)
		SetDockingStatus(EDockingStatus::None);
		
		// 2. Avisa a Nave que o sinal caiu (Esconde o TXT_AlertSitcher!)
		Ship->UpdateDockingSignal(ESignalStatus::Lost);
	}
}

EDockingStatus ASpaceDockingPort::RequestDockingPermission(float ApproachSpeedMPS)
{
	EDockingStatus Result = (ApproachSpeedMPS <= MaxSafeDockingSpeed) ? EDockingStatus::Granted : EDockingStatus::Denied;
	SetDockingStatus(Result);
	return Result;
}

void ASpaceDockingPort::OnPrecisionVolumeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. Uma nave encostou na âncora?
	if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(OtherActor))
	{
		// 2. A nave está em modo de pouso (Grampos armados)?
		if (Ship->GetCurrentFlightState() == EShipFlightState::LandingMode)
		{
			// 3. Verifica a Velocidade
			float Speed = Ship->GetVelocity().Size() / 100.0f;
			bool bIsSpeedOk = (Speed <= MaxSafeDockingSpeed);

			// 4. A CORREÇÃO: O Segurança verifica o alinhamento da nave (Nose E Roll)!
			bool bIsAlignmentOk = false;
            
			if (USpaceTelemetryComponent* Telemetry = Ship->FindComponentByClass<USpaceTelemetryComponent>())
			{
				FSpaceTelemetryData Data = Telemetry->GetLatestTelemetry();

				// REGRA DE OURO JUSTA: Ambos têm que ser maiores que 95%
				if (Data.NoseAlignmentPercent >= 95.0f && Data.RollAlignmentPercent >= 95.0f)
				{
					bIsAlignmentOk = true;
				}
			}

			// 5. O Veredito Final
			if (bIsSpeedOk && bIsAlignmentOk)
			{
				// Alfândega liberou! Puxa o gatilho da nave.
				SetDockingStatus(EDockingStatus::Granted);
				Ship->ExecuteHardcoreDocking(this);
			}
			else
			{
				// O jogador bateu. Ou velocidade alta, ou estava torto.
				SetDockingStatus(EDockingStatus::Denied);
				UE_LOG(LogTemp, Warning, TEXT("Docking Negado! Velocidade OK: %d | Alinhamento OK: %d"), bIsSpeedOk, bIsAlignmentOk);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Falha: A nave encostou no berço, mas não estava em Landing Mode!"));
		}
	}
}
