#include "DockingPort/SpaceDockingPort.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "Player/SpaceShipPawn.h" 

ASpaceDockingPort::ASpaceDockingPort()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));

	DockStructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DockStructureMesh"));
	DockStructureMesh->SetupAttachment(RootComponent);

	DockingAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("DockingAnchor"));
	DockingAnchor->SetupAttachment(DockStructureMesh);

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
    
	PrecisionDockingVolume->SetBoxExtent(FVector(100.0f, 100.0f, 50.0f)); 
	PrecisionDockingVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PrecisionDockingVolume->SetCollisionResponseToAllChannels(ECR_Overlap);
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
		// 2. A NAVE ESTÁ EM MODO DE POUSO? (Substitui o bool bAreClampsArmed)
		if (Ship->GetCurrentFlightState() == EShipFlightState::LandingMode)
		{
			// 3. A velocidade está correta?
			float Speed = Ship->GetVelocity().Size() / 100.0f;
			EDockingStatus Status = RequestDockingPermission(Speed);

			// 4. O Veredito!
			if (Status == EDockingStatus::Granted)
			{
				// A Alfândega liberou! Puxa o gatilho da nave para desligar os motores.
				Ship->ExecuteHardcoreDocking(this);
			}
		}
		else
		{
			// O jogador bateu a nave na estação porque não apertou o botão de armar os grampos!
			UE_LOG(LogTemp, Error, TEXT("Falha: A nave encostou no berço, mas não estava em Landing Mode!"));
		}
	}
}
