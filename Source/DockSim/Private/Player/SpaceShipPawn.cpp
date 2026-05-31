#include "Player/SpaceShipPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CFTVComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpaceHullComponent.h"
#include "Components/SpaceMovementComponent.h"
#include "Components/SpaceResourceComponent.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Components/WidgetComponent.h" 
#include "Core/SpacePlayerController.h"
#include "DockingPort/SpaceDockingPort.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SpotLightComponent.h"
#include "UI/CameraHUDWidget.h"
#include "Components/SpaceAudioComponent.h"
#include "UI/ShipHUDWidget.h"

ASpaceShipPawn::ASpaceShipPawn()
{
	PrimaryActorTick.bCanEverTick = true;	
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMesh;
	
	CockpitCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CockpitCamera"));
	CockpitCamera->SetupAttachment(ShipMesh);
	
	MovementComponent = CreateDefaultSubobject<USpaceMovementComponent>(TEXT("MovementComponent"));
	TelemetryComponent = CreateDefaultSubobject<USpaceTelemetryComponent>(TEXT("TelemetryComponent"));
	ResourceComponent = CreateDefaultSubobject<USpaceResourceComponent>(TEXT("ResourceComponent"));
	HullComponent = CreateDefaultSubobject<USpaceHullComponent>(TEXT("HullComponent"));
	CFTVSystem = CreateDefaultSubobject<UCFTVComponent>(TEXT("CFTV_System"));
	AudioSystem = CreateDefaultSubobject<USpaceAudioComponent>(TEXT("AudioSystem"));
	
	DockingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DockingPoint"));
	DockingPoint->SetupAttachment(ShipMesh);

	Cam_Docking = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Cam_Docking"));
	Cam_Docking->SetupAttachment(DockingPoint);
	Cam_Docking->bCaptureEveryFrame = false;
	Cam_Docking->bCaptureOnMovement = false;
	Cam_Docking->CaptureSource = SCS_FinalColorLDR;
	
	SensorShield = CreateDefaultSubobject<USphereComponent>(TEXT("SensorShield"));
	SensorShield->SetupAttachment(ShipMesh);
	SensorShield->InitSphereRadius(500.0f); 
	SensorShield->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SensorShield->SetGenerateOverlapEvents(true);

	LeftHeadlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LeftHeadlight"));
	LeftHeadlight->SetupAttachment(ShipMesh);
	LeftHeadlight->SetRelativeLocation(FVector(200.0f, -150.0f, 0.0f)); 
	LeftHeadlight->Intensity = 500000.0f; 
	LeftHeadlight->AttenuationRadius = 10000.0f;
	LeftHeadlight->InnerConeAngle = 20.0f;
	LeftHeadlight->OuterConeAngle = 45.0f; 
	LeftHeadlight->SetVisibility(false);
	LeftHeadlight->bUseInverseSquaredFalloff = false; 
	LeftHeadlight->SetCastShadows(false); 

	RightHeadlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RightHeadlight"));
	RightHeadlight->SetupAttachment(ShipMesh);
	RightHeadlight->SetRelativeLocation(FVector(200.0f, 150.0f, 0.0f));
	RightHeadlight->Intensity = 500000.0f;
	RightHeadlight->AttenuationRadius = 10000.0f;
	RightHeadlight->bUseInverseSquaredFalloff = false;
	RightHeadlight->SetCastShadows(false);
	RightHeadlight->InnerConeAngle = 20.0f;
	RightHeadlight->OuterConeAngle = 45.0f;
	RightHeadlight->SetVisibility(false);
}

void ASpaceShipPawn::BeginPlay()
{
	Super::BeginPlay();

	if (CFTVSystem)
	{
		CFTVSystem->InitializeDockingCamera(Cam_Docking);
	}
	
	if (ShipMesh)
	{
		ShipMesh->OnComponentHit.AddDynamic(this, &ASpaceShipPawn::OnShipHit);
	}

	if (HullComponent)
	{
		HullComponent->OnHullWarning.AddDynamic(this, &ASpaceShipPawn::HandleHullWarning);
		HullComponent->OnHullCritical.AddDynamic(this, &ASpaceShipPawn::HandleHullCritical);
	}
	
	if (SensorShield)
	{
		SensorShield->OnComponentBeginOverlap.AddDynamic(this, &ASpaceShipPawn::OnSensorOverlap);
	}

	DynamicDockingRT = NewObject<UTextureRenderTarget2D>(this);
	DynamicDockingRT->RenderTargetFormat = RTF_RGBA8;
	DynamicDockingRT->InitAutoFormat(1024, 1024); 
	DynamicDockingRT->UpdateResourceImmediate(true);

	if (Cam_Docking)
	{
		Cam_Docking->TextureTarget = DynamicDockingRT;
		Cam_Docking->HiddenActors.Add(this);
	}
	
	if (HUDClass)
	{
		MainHUDInstance = CreateWidget<UShipHUDWidget>(GetWorld(), HUDClass);
		
		if (MainHUDInstance)
		{
			MainHUDInstance->AddToViewport();
			MainHUDInstance->SetDockingHUDVisible(false);
		}
	}
	
}
	
void ASpaceShipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CockpitCamera) return;

	if (CurrentVisionMode != EPilotVisionMode::FreeLook)
	{
		FRotator CurrentHeadRot = CockpitCamera->GetRelativeRotation();
    
		if (!CurrentHeadRot.Equals(FRotator::ZeroRotator, 0.1f))
		{
			FRotator SmoothReturn = FMath::RInterpTo(CurrentHeadRot, FRotator::ZeroRotator, DeltaTime, 10.0f);
			CockpitCamera->SetRelativeRotation(SmoothReturn);
		}
	}
}

void ASpaceShipPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpaceShipPawn::Move);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASpaceShipPawn::Move);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled,  this, &ASpaceShipPawn::Move);
		}
		if (ThrottleAction)
		{
			EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceShipPawn::OnThrottle);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpaceShipPawn::Look);
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ASpaceShipPawn::Look);
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Canceled, this, &ASpaceShipPawn::Look);
		}
		if (RollAction)
		{
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ASpaceShipPawn::OnRoll);
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &ASpaceShipPawn::OnRoll);
			EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Canceled, this, &ASpaceShipPawn::OnRoll);
		}
		if (PrecisionAction)
		{
			EnhancedInputComponent->BindAction(PrecisionAction, ETriggerEvent::Started,   this, &ASpaceShipPawn::OnPrecisionModeStart);
			EnhancedInputComponent->BindAction(PrecisionAction, ETriggerEvent::Completed, this, &ASpaceShipPawn::OnPrecisionModeEnd);
			EnhancedInputComponent->BindAction(PrecisionAction, ETriggerEvent::Canceled, this, &ASpaceShipPawn::OnPrecisionModeEnd);
		}
		if (FreeLookAction)
		{
			EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Started, this, &ASpaceShipPawn::ToggleFreeLook);
		}
		if (DockAction)
		{
			EnhancedInputComponent->BindAction(DockAction, ETriggerEvent::Started, this, &ASpaceShipPawn::OnDockButtonPressed);
		}
		if (PauseAction)
		{
			EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ASpaceShipPawn::OnPausePressed);
		}
		if (ToggleHeadlightsAction)
		{
			EnhancedInputComponent->BindAction(ToggleHeadlightsAction, ETriggerEvent::Started, this, &ASpaceShipPawn::OnToggleHeadlights);
		}
	}
}

void ASpaceShipPawn::OnThrottle(const FInputActionValue& Value)
{
	float ScrollValue = Value.Get<float>();
	if (CurrentFlightState == EShipFlightState::Docked || CurrentFlightState == EShipFlightState::Disabled) return;
	if (MovementComponent)
	{
		MovementComponent->AdjustThrottle(ScrollValue * 0.1f); 
	}
}

void ASpaceShipPawn::Move(const FInputActionValue& Value)
{
	FVector MoveAxisVector = Value.Get<FVector>();
	if (CurrentFlightState == EShipFlightState::Docked || CurrentFlightState == EShipFlightState::Disabled) return;
	if (MovementComponent)
	{
		float ForwardInput = MoveAxisVector.Y; 
		float RightInput   = MoveAxisVector.X; 
		float UpInput      = MoveAxisVector.Z; 

		MovementComponent->RequestTranslation(FVector(ForwardInput, RightInput, UpInput));
	}
}

void ASpaceShipPawn::OnRoll(const FInputActionValue& Value)
{
	float RollInput = Value.Get<float>();
    
	if (CurrentFlightState == EShipFlightState::Docked || CurrentFlightState == EShipFlightState::Disabled) return;

	if (MovementComponent)
	{
		MovementComponent->RequestRotation(FVector(0.0f, 0.0f, RollInput));
	}
}

void ASpaceShipPawn::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
	
    if (CurrentVisionMode == EPilotVisionMode::FreeLook)
    {
        FRotator CurrentHeadRot = CockpitCamera->GetRelativeRotation();

        float NewPitch = CurrentHeadRot.Pitch + (LookAxisVector.Y * LookSensitivity);
        CurrentHeadRot.Pitch = FMath::Clamp(NewPitch, MaxLookDown, MaxLookUp);

        float NewYaw = CurrentHeadRot.Yaw + (LookAxisVector.X * LookSensitivity);
        CurrentHeadRot.Yaw = FMath::Clamp(NewYaw, -MaxLookLeftRight, MaxLookLeftRight);

        CockpitCamera->SetRelativeRotation(CurrentHeadRot);
        return; 
    }
	
	if (CurrentFlightState == EShipFlightState::Docked || CurrentFlightState == EShipFlightState::Disabled) return;

	if (MovementComponent)
	{
		float PitchInput = LookAxisVector.Y; 
		float YawInput = LookAxisVector.X; 
        
		MovementComponent->RequestRotation(FVector(PitchInput, YawInput, 0.0f));
	}
}

void ASpaceShipPawn::OnPrecisionModeStart(const FInputActionValue& Value)
{
	if (MovementComponent) MovementComponent->SetThrustPowerMode(EThrustPowerMode::Precision);
}

void ASpaceShipPawn::OnPrecisionModeEnd(const FInputActionValue& Value)
{
	if (MovementComponent) MovementComponent->SetThrustPowerMode(EThrustPowerMode::Standard);
}

void ASpaceShipPawn::ToggleFreeLook(const FInputActionValue& Value)
{
	CurrentVisionMode = (CurrentVisionMode == EPilotVisionMode::FreeLook) ? EPilotVisionMode::Standard : EPilotVisionMode::FreeLook;
}

void ASpaceShipPawn::UpdateDockingSignal(ESignalStatus NewStatus)
{
	CurrentSignalStatus = NewStatus;

	if (ASpacePlayerController* PC = Cast<ASpacePlayerController>(GetController()))
	{
		PC->UpdateSignalAlert(CurrentSignalStatus);
	}
}

void ASpaceShipPawn::OnDockButtonPressed(const FInputActionValue& Value)
{
    if (CurrentFlightState == EShipFlightState::Disabled) return;
	
    switch (CurrentFlightState)
    {
        case EShipFlightState::Docked:
            ShipMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            ShipMesh->SetSimulatePhysics(true);
            ShipMesh->WakeAllRigidBodies();

            CurrentFlightState = EShipFlightState::FreeFlying;
    	if (MainHUDInstance) MainHUDInstance->SetDockingHUDVisible(false);

    case EShipFlightState::FreeFlying:
    	CurrentFlightState = EShipFlightState::LandingMode;
    
    	if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = true;

    	if (MainHUDInstance) 
    	{
    		MainHUDInstance->SetDockingHUDVisible(true);
    		MainHUDInstance->SetDockingVideo(DynamicDockingRT);
    	}

    	GetWorldTimerManager().SetTimer(TelemetryTimerHandle, this, &ASpaceShipPawn::UpdateDockingMonitor, 0.1f, true);
    	break;

    case EShipFlightState::LandingMode:
    	CurrentFlightState = EShipFlightState::FreeFlying;
            
    	if (MainHUDInstance) MainHUDInstance->SetDockingHUDVisible(false);
    	if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = false;

    	GetWorldTimerManager().ClearTimer(TelemetryTimerHandle);

    	break;
    }
}

void ASpaceShipPawn::ExecuteHardcoreDocking(ASpaceDockingPort* TargetPort)
{
	ShipMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	ShipMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	ShipMesh->SetSimulatePhysics(false);

	FAttachmentTransformRules DockingRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
	ShipMesh->AttachToComponent(TargetPort->DockingAnchor, DockingRules, FName("DockingPoint"));

	CurrentFlightState = EShipFlightState::Docked;
	
	if (MainHUDInstance) MainHUDInstance->SetDockingHUDVisible(false);
	if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = false;

	if (ASpacePlayerController* PC = Cast<ASpacePlayerController>(GetController()))
	{
		PC->SetMissionState(EMissionState::Victory);
	}
	if (AudioSystem)
	{
		AudioSystem->PlayDockingSuccessSound();
	}
	GetWorldTimerManager().ClearTimer(TelemetryTimerHandle);
	OnDockingCompleted.Broadcast();
}

void ASpaceShipPawn::OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this) return;
	PreImpactVelocity = ShipMesh->GetComponentVelocity();
}

void ASpaceShipPawn::OnShipHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (CurrentFlightState == EShipFlightState::Docked) return;
	
	FVector RelativeVelocity = PreImpactVelocity;
	if (OtherComp && OtherComp->Mobility != EComponentMobility::Static) 
	{
		RelativeVelocity -= OtherComp->GetComponentVelocity(); 
	}

	float ImpactSpeedCM = FMath::Abs(FVector::DotProduct(RelativeVelocity, -Hit.ImpactNormal));
	float ImpactSpeedMPS = ImpactSpeedCM / 100.0f;

	if (HullComponent)
	{
		HullComponent->ProcessImpact(ImpactSpeedMPS);
	}
}

void ASpaceShipPawn::HandleHullWarning(float ImpactSpeed)
{
	if (AudioSystem)
	{
		AudioSystem->PlayShipCrashSound();
	}
}

void ASpaceShipPawn::HandleHullCritical()
{
	CurrentFlightState = EShipFlightState::Disabled;

	if (ShipMesh)
	{
		ShipMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		float SpinX = FMath::RandRange(-30.0f, 30.0f);
		float SpinY = FMath::RandRange(-30.0f, 30.0f);
		float SpinZ = FMath::RandRange(-30.0f, 30.0f);
		ShipMesh->SetPhysicsAngularVelocityInDegrees(FVector(SpinX, SpinY, SpinZ));
	}
	if (ASpacePlayerController* PC = Cast<ASpacePlayerController>(GetController()))
	{
		PC->SetMissionState(EMissionState::GameOver);
	}
	
	if (AudioSystem)
	{
		AudioSystem->PlayDockingFailSound();
	}
	GetWorldTimerManager().ClearTimer(TelemetryTimerHandle);
}
void ASpaceShipPawn::OnPausePressed(const FInputActionValue& Value)
{
	if (ASpacePlayerController* PC = Cast<ASpacePlayerController>(GetController()))
	{
		PC->TogglePauseMenu();
	}
}

void ASpaceShipPawn::UpdateDockingMonitor()
{
	if (!TelemetryComponent || !MainHUDInstance || CurrentFlightState != EShipFlightState::LandingMode) return;

	FSpaceTelemetryData CurrentData = TelemetryComponent->GetLatestTelemetry();

	FString StatusText = TEXT("[ DOCKING MODE ACTIVATED ]");
	FLinearColor TextColor = FLinearColor::Yellow;

	if (CurrentData.VelTotal > MaxSafeImpactSpeed)
	{
		StatusText = TEXT("[ WARNING: HIGH VELOCITY ]");
		TextColor = FColor::FromHex("#FFB347");
	}
	else if (CurrentData.NoseAlignmentPercent > 95.0f && CurrentData.RollAlignmentPercent > 95.0f && CurrentData.Distance < 2.0f)
	{
		StatusText = TEXT("[ PERFECT ALIGNMENT ]");
		TextColor = FColor::FromHex("#38FF9C");
	}

	MainHUDInstance->UpdateDockingTelemetry(
		CurrentData.Distance, 
		CurrentData.RelVelTotal, 
		CurrentData.NoseAlignmentPercent, 
		CurrentData.RollAlignmentPercent, 
		StatusText, 
		TextColor
	);
}

void ASpaceShipPawn::OnToggleHeadlights(const FInputActionValue& Value)
{
	if (CurrentFlightState == EShipFlightState::Disabled) return;

	bHeadlightsOn = !bHeadlightsOn;

	if (LeftHeadlight)
	{
		LeftHeadlight->SetVisibility(bHeadlightsOn);
	}

	if (RightHeadlight)
	{
		RightHeadlight->SetVisibility(bHeadlightsOn);
	}
}