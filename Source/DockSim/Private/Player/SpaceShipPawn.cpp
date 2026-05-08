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
#include "UI/DockingHUDWidget.h"
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

	// ==========================================
	// MONITORES FÍSICOS 3D
	// ==========================================
	
	LeftHeadlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LeftHeadlight"));
	LeftHeadlight->SetupAttachment(ShipMesh);
	// Move a luz um pouco para frente (X) e para a esquerda (Y)
	LeftHeadlight->SetRelativeLocation(FVector(200.0f, -150.0f, 0.0f)); 
	LeftHeadlight->Intensity = 500000.0f; // Força no vácuo
	LeftHeadlight->AttenuationRadius = 10000.0f;
	LeftHeadlight->InnerConeAngle = 20.0f; // Foco central forte
	LeftHeadlight->OuterConeAngle = 45.0f; // Penumbra suave
	LeftHeadlight->SetVisibility(false); // Começa desligado para economizar bateria!
	LeftHeadlight->bUseInverseSquaredFalloff = false; // Torna o alcance mais "linear" e previsível
	LeftHeadlight->SetCastShadows(false); // Faróis de nave geralmente não precisam projetar sombras do cenário para o MVP

	RightHeadlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RightHeadlight"));
	RightHeadlight->SetupAttachment(ShipMesh);
	// Move a luz um pouco para frente (X) e para a direita (Y)
	RightHeadlight->SetRelativeLocation(FVector(200.0f, 150.0f, 0.0f));
	RightHeadlight->Intensity = 500000.0f; // Força no vácuo
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
	
	if (HUDClass)
	{
		// 2. Cria o Widget de fato na memória e preenche o nosso ponteiro!
		MainHUDInstance = CreateWidget<UShipHUDWidget>(GetWorld(), HUDClass);
		
		if (MainHUDInstance)
		{
			// 3. Adiciona na tela do jogador
			MainHUDInstance->AddToViewport();
			
			// 4. Garante que o painel de docking comece invisível por padrão
			MainHUDInstance->SetDockingHUDVisible(false);
		}
	}
	else
	{
		// Um aviso amigável para você não esquecer de setar a variável no Editor
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("ERRO: HUDClass não configurada no BP_SpaceShipPawn!"));
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

            if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("TRAVAS LIBERADAS. Voo Livre."));
            break;

    case EShipFlightState::FreeFlying:
    	CurrentFlightState = EShipFlightState::LandingMode;
    
    	if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = true;

    	// ✔️ Avisa o HUD para aparecer
    	if (MainHUDInstance) 
    	{
    		MainHUDInstance->SetDockingHUDVisible(true);
    		MainHUDInstance->SetDockingVideo(DynamicDockingRT); // A entrega da fita de vídeo!
    	}

    	GetWorldTimerManager().SetTimer(TelemetryTimerHandle, this, &ASpaceShipPawn::UpdateDockingMonitor, 0.1f, true);
    	break;

    case EShipFlightState::LandingMode:
    	CurrentFlightState = EShipFlightState::FreeFlying;
            
    	if (MainHUDInstance) MainHUDInstance->SetDockingHUDVisible(false);
    	if (Cam_Docking) Cam_Docking->bCaptureEveryFrame = false;

    	// DESLIGA O TIMER: Para de gastar CPU imediatamente
    	GetWorldTimerManager().ClearTimer(TelemetryTimerHandle);

    	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("MODO DE POUSO ABORTADO."));
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
	if (AudioSystem)
	{
		AudioSystem->PlayShipCrashSound();
	}
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
	// 1. CHECAGEM DE SEGURANÇA: 
	// Em vez de checar o monitor 3D, checamos se o HUD existe e se estamos no modo de pouso.
	if (!TelemetryComponent || !MainHUDInstance || CurrentFlightState != EShipFlightState::LandingMode) return;

	// 2. BUSCA OS DADOS (Lógica mantida, pois está ótima)
	FSpaceTelemetryData CurrentData = TelemetryComponent->GetLatestTelemetry();

	FString StatusText = TEXT("[ MODO DE POUSO ATIVO ]");
	FLinearColor TextColor = FLinearColor::Yellow;

	if (CurrentData.VelTotal > MaxSafeImpactSpeed)
	{
		StatusText = TEXT("[ PERIGO: ALTA VELOCIDADE DE APROXIMAÇÃO ]");
		TextColor = FLinearColor::Red;
	}
	else if (CurrentData.NoseAlignmentPercent > 95.0f && CurrentData.RollAlignmentPercent > 95.0f && CurrentData.Distance < 2.0f)
	{
		StatusText = TEXT("[ ALINHAMENTO PERFEITO: MANTENHA ]");
		TextColor = FLinearColor::Green;
	}

	// 3. ENVIA PARA O HUD
	// Substituímos o "Monitor3D" pelo nosso MainHUDInstance
	MainHUDInstance->UpdateDockingTelemetry(
		CurrentData.Distance, 
		CurrentData.VelTotal, 
		CurrentData.NoseAlignmentPercent, 
		CurrentData.RollAlignmentPercent, 
		StatusText, 
		TextColor
	);
    
	// Debug mantido para facilitar seus testes no editor
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Cyan, FString::Printf(TEXT("Bico: %.1f %% | Eixo: %.1f %%"), CurrentData.NoseAlignmentPercent, CurrentData.RollAlignmentPercent));
	}
}

void ASpaceShipPawn::OnToggleHeadlights(const FInputActionValue& Value)
{
	// Se a nave estiver morta, não tem energia para a luz
	if (CurrentFlightState == EShipFlightState::Disabled) return;

	// O método nativo da Unreal que inverte de True para False automaticamente
	if (LeftHeadlight) LeftHeadlight->ToggleVisibility();
	if (RightHeadlight) RightHeadlight->ToggleVisibility();

	if (GEngine)
	{
		FString State = (LeftHeadlight && LeftHeadlight->IsVisible()) ? TEXT("ON") : TEXT("OFF");
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("SISTEMA: Faróis %s"), *State));
	}
}