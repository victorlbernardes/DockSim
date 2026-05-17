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

	DockVisualRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DockVisualRing"));
	DockVisualRing->SetupAttachment(DockingAnchor);
	DockVisualRing->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> TargetMesh(TEXT("/Engine/BasicShapes/Cylinder"));
	if (TargetMesh.Succeeded())
	{
		DockVisualRing->SetStaticMesh(TargetMesh.Object);
		DockVisualRing->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.1f));
		DockVisualRing->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f)); 
	}

	SirenBulb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SirenBulb"));
	SirenBulb->SetupAttachment(DockingAnchor);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMesh.Succeeded())
	{
		SirenBulb->SetStaticMesh(SphereMesh.Object);
		SirenBulb->SetRelativeScale3D(FVector(0.15f));
	}
	SirenBulb->SetCollisionProfileName(TEXT("NoCollision"));

	BeaconLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BeaconLight"));
	BeaconLight->SetupAttachment(SirenBulb); 
	BeaconLight->SetMobility(EComponentMobility::Movable);
	BeaconLight->SetAttenuationRadius(2500.0f); 
	BeaconLight->bUseInverseSquaredFalloff = false; 

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

	for (int32 i = 0; i < NumberOfLights; i++)
	{
		FName LightName = FName(*FString::Printf(TEXT("RunwayLight_%d"), i));
		
		UStaticMeshComponent* LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(LightName);
		LightMesh->SetupAttachment(DockingAnchor);
		
		if (SphereMeshLoader.Succeeded())
		{
			LightMesh->SetStaticMesh(SphereMeshLoader.Object);
		}
		LightMesh->SetRelativeScale3D(FVector(0.1f)); 
		LightMesh->SetCollisionProfileName(TEXT("NoCollision")); 
		
		float AngleRads = (i * (360.0f / NumberOfLights)) * (PI / 180.0f);
		
		float YPos = FMath::Cos(AngleRads) * RunwayRingRadius;
		float ZPos = FMath::Sin(AngleRads) * RunwayRingRadius;
		
		LightMesh->SetRelativeLocation(FVector(0.0f, YPos, ZPos));

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
			Color = FLinearColor(0.0f, 0.5f, 1.0f); 
			Alpha = (FMath::Sin(Time * 2.0f) + 1.0f) * 0.5f;
			break;

		case EDockingStatus::Standby:
			Color = FLinearColor(1.0f, 1.0f, 0.0f);
			Alpha = (FMath::Fmod(Time, 0.8f) < 0.4f) ? 1.0f : 0.0f; 
			break;

		case EDockingStatus::Granted:
			Color = FLinearColor(0.0f, 1.0f, 0.0f);
			Alpha = 1.0f; 
			break;

		case EDockingStatus::Denied:
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
		SetDockingStatus(EDockingStatus::Standby);
		
		Ship->UpdateDockingSignal(ESignalStatus::Acquired);
	}
}

void ASpaceDockingPort::OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(OtherActor))
	{
		SetDockingStatus(EDockingStatus::None);
		
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
	if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(OtherActor))
	{
		if (Ship->GetCurrentFlightState() == EShipFlightState::LandingMode)
		{
			float Speed = Ship->GetVelocity().Size() / 100.0f;
			bool bIsSpeedOk = (Speed <= MaxSafeDockingSpeed);

			bool bIsAlignmentOk = false;
            
			if (USpaceTelemetryComponent* Telemetry = Ship->FindComponentByClass<USpaceTelemetryComponent>())
			{
				FSpaceTelemetryData Data = Telemetry->GetLatestTelemetry();

				if (Data.NoseAlignmentPercent >= 95.0f && Data.RollAlignmentPercent >= 95.0f)
				{
					bIsAlignmentOk = true;
				}
			}

			if (bIsSpeedOk && bIsAlignmentOk)
			{
				SetDockingStatus(EDockingStatus::Granted);
				Ship->ExecuteHardcoreDocking(this);
			}
			else
			{
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
