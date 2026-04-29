#include "Player/SpaceShipPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
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
#include "Kismet/GameplayStatics.h"
#include "UI/DockingHUDWidget.h"
#include "DrawDebugHelpers.h"
#include "Components/SpaceAudioComponent.h"

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

	// ==========================================
	// MONITORES FÍSICOS 3D
	// ==========================================
	// 1. A Carcaça do Monitor (Plástico)
	DockingScreenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DockingScreenMesh"));
	DockingScreenMesh->SetupAttachment(ShipMesh); // Anexado à nave (ou à câmera se preferir que siga o rosto)

	// 2. A Tela Digital (Widget)
	DockingMonitorComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DockingMonitorComp"));
	DockingMonitorComp->SetupAttachment(DockingScreenMesh); // Anexado à carcaça do monitor
	DockingMonitorComp->SetWidgetSpace(EWidgetSpace::World);
	DockingMonitorComp->SetDrawSize(FVector2D(1024.0f, 1024.0f)); // Resolução alta
	DockingMonitorComp->SetVisibility(false); // Começa desligado
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

	// ==========================================
	// CÂMERA DINÂMICA LIGADA AO MONITOR 3D
	// ==========================================
	
	// 1. Cria o Render Target de forma dinâmica e segura
	DynamicDockingRT = NewObject<UTextureRenderTarget2D>(this);
	DynamicDockingRT->RenderTargetFormat = RTF_RGBA8;
	DynamicDockingRT->InitAutoFormat(1024, 1024); 
	DynamicDockingRT->UpdateResourceImmediate(true);

	// 2. Conecta a câmara da nave ao Render Target
	if (Cam_Docking)
	{
		Cam_Docking->TextureTarget = DynamicDockingRT;
		Cam_Docking->HiddenActors.Add(this);
	}

	// 3. Puxa a interface de dentro do componente 3D e injeta o vídeo nela
	if (DockingMonitorComp)
	{
		if (UDockingHUDWidget* Monitor3D = Cast<UDockingHUDWidget>(DockingMonitorComp->GetUserWidgetObject()))
		{
			Monitor3D->SetCameraFeed(DynamicDockingRT);
		}
	}
}

void ASpaceShipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CockpitCamera) return;

	// Regresso suave da cabeça (Free Look)
	if (CurrentVisionMode != EPilotVisionMode::FreeLook)
	{
		FRotator CurrentHeadRot = CockpitCamera->GetRelativeRotation();
    
		if (!CurrentHeadRot.Equals(FRotator::ZeroRotator, 0.1f))
		{
			FRotator SmoothReturn = FMath::RInterpTo(CurrentHeadRot, FRotator::ZeroRotator, DeltaTime, 10.0f);
			CockpitCamera->SetRelativeRotation(SmoothReturn);
		}
	}

	// ==========================================
	// ATUALIZAÇÃO DA TELEMETRIA NO MONITOR 3D
	// ==========================================
	if (CurrentFlightState == EShipFlightState::LandingMode && DockingMonitorComp && DockingMonitorComp->IsVisible())
	{
		// 1. Pega TUDO do componente de telemetria
		FSpaceTelemetryData CurrentData = TelemetryComponent->GetLatestTelemetry();

		FString StatusText = TEXT("[ MODO DE POUSO ATIVO ]");
		FLinearColor TextColor = FLinearColor::Yellow;

		if (CurrentData.VelTotal > MaxSafeImpactSpeed)
		{
			StatusText = TEXT("[ PERIGO: ALTA VELOCIDADE DE APROXIMAÇÃO ]");
			TextColor = FLinearColor::Red;
		}
		else if (CurrentData.TotalAlignment > 95.0f && CurrentData.Distance < 2.0f)
		{
			StatusText = TEXT("[ ALINHAMENTO PERFEITO: MANTENHA ]");
			TextColor = FLinearColor::Green;
		}

		// 2. Envia os dados para a tela 3D (ATENÇÃO: Garanta que no Widget a função receba o Alignment!)
		if (UDockingHUDWidget* Monitor3D = Cast<UDockingHUDWidget>(DockingMonitorComp->GetUserWidgetObject()))
		{
			Monitor3D->UpdateTelemetry(CurrentData.Distance, CurrentData.VelTotal, CurrentData.TotalAlignment, StatusText, TextColor);
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
		}
		if (ToggleYawAction)
		{
			EnhancedInputComponent->BindAction(ToggleYawAction, ETriggerEvent::Started, this, &ASpaceShipPawn::OnToggleYaw);
		}
		if (ToggleRollAction)
		{
			EnhancedInputComponent->BindAction(ToggleRollAction, ETriggerEvent::Started, this, &ASpaceShipPawn::OnToggleRoll);
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
		if (EscAction)
		{
			EnhancedInputComponent->BindAction(EscAction, ETriggerEvent::Started, this, &ASpaceShipPawn::OnPausePressed);
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

void ASpaceShipPawn::OnToggleYaw(const FInputActionValue& Value)
{
	if (CurrentFlightState == EShipFlightState::Docked || CurrentFlightState == EShipFlightState::Disabled) return;
    switch (CurrentRotState)
    {
        case ERotationMode::Locked:      CurrentRotState = ERotationMode::YawControl; break;
        case ERotationMode::RollControl:  CurrentRotState = ERotationMode::YawAndRollControl; break; 
        case ERotationMode::YawControl:   CurrentRotState = ERotationMode::Locked; break;     
        case ERotationMode::YawAndRollControl:  CurrentRotState = ERotationMode::RollControl; break; 
    }
}

void ASpaceShipPawn::OnToggleRoll(const FInputActionValue& Value)
{
	if (CurrentFlightState == EShipFlightState::Docked || CurrentFlightState == EShipFlightState::Disabled) return;
    switch (CurrentRotState)
    {
        case ERotationMode::Locked:      CurrentRotState = ERotationMode::RollControl; break;
        case ERotationMode::YawControl:   CurrentRotState = ERotationMode::YawAndRollControl; break;
        case ERotationMode::RollControl:  CurrentRotState = ERotationMode::Locked; break;
        case ERotationMode::YawAndRollControl:  CurrentRotState = ERotationMode::YawControl; break;
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
        float YawInput = 0.0f;
        float RollInput = 0.0f;
    	
        switch (CurrentRotState)
        {
            case ERotationMode::YawControl:
                YawInput = LookAxisVector.X;
                break;
            case ERotationMode::RollControl:
                RollInput = -LookAxisVector.X;
                break;
            case ERotationMode::YawAndRollControl:
                YawInput = LookAxisVector.X;
                RollInput = -LookAxisVector.X;
                break;
            case ERotationMode::Locked:
                break;
        }
    	MovementComponent->RequestRotation(FVector(PitchInput, YawInput, RollInput));
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
            if (DockingMonitorComp) DockingMonitorComp->SetVisibility(false);

            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("TRAVAS LIBERADAS. Voo Livre."));
            break;

        case EShipFlightState::FreeFlying:
            CurrentFlightState = EShipFlightState::LandingMode;
            
            // LIGA O MONITOR FÍSICO E A CÂMARA
            if (DockingMonitorComp) DockingMonitorComp->SetVisibility(true);
            if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = true;

            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("MODO DE POUSO: Grampos armados. Aproxime-se do berço."));
            break;

        case EShipFlightState::LandingMode:
            CurrentFlightState = EShipFlightState::FreeFlying;
            
            // DESLIGA O MONITOR FÍSICO E A CÂMARA
            if (DockingMonitorComp) DockingMonitorComp->SetVisibility(false);
            if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = false;

            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("MODO DE POUSO ABORTADO. Grampos recolhidos."));
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
	
	// Desliga a tela do painel após o docking ser concluído
	if (DockingMonitorComp) DockingMonitorComp->SetVisibility(false);
	if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = false;

	if (ASpacePlayerController* PC = Cast<ASpacePlayerController>(GetController()))
	{
		PC->SetMissionState(EMissionState::Victory);
	}
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("DOCKING PERFEITO!"));
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

	if (GEngine) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("ESCUDO SENSOR: Impacto calculado a %.1f m/s"), ImpactSpeedMPS));
	}

	if (HullComponent)
	{
		HullComponent->ProcessImpact(ImpactSpeedMPS);
	}
}

void ASpaceShipPawn::HandleHullWarning(float ImpactSpeed)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("ALERTA DA NAVE: Casco avariado a %.1f m/s."), ImpactSpeed));
}

void ASpaceShipPawn::HandleHullCritical()
{
	CurrentFlightState = EShipFlightState::Disabled;

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 8.0f, FColor::Red, TEXT("SISTEMAS CRÍTICOS: Perda total de controlo de voo."));

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
}
void ASpaceShipPawn::OnPausePressed(const FInputActionValue& Value)
{
	if (ASpacePlayerController* PC = Cast<ASpacePlayerController>(GetController()))
	{
		PC->TogglePauseMenu();
	}
}