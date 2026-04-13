#include "Player/SpaceShipPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Components/CFTVComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SpaceMovementComponent.h"
#include "Components/SpaceResourceComponent.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Components/WidgetComponent.h"


ASpaceShipPawn::ASpaceShipPawn()
{
	PrimaryActorTick.bCanEverTick = true;	
	
	ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
    
	// 2. Definimos a Malha como a "Raiz" (O centro físico de gravidade e colisão do Pawn)
	RootComponent = ShipMesh;

	// 3. Criamos a Câmera do Cockpit
	CockpitCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CockpitCamera"));
    
	// 4. Anexamos (colamos) a câmera na malha. Se a nave girar, a câmera gira junto!
	CockpitCamera->SetupAttachment(ShipMesh);
	
	MovementComponent = CreateDefaultSubobject<USpaceMovementComponent>(TEXT("MovementComponent"));
	TelemetryComponent = CreateDefaultSubobject<USpaceTelemetryComponent>(TEXT("TelemetryComponent"));
	ResourceComponent = CreateDefaultSubobject<USpaceResourceComponent>(TEXT("ResourceComponent"));
	CFTVSystem = CreateDefaultSubobject<UCFTVComponent>(TEXT("CFTV_System"));
	
	//Criação Física das Câmeras (Todas nascem DESLIGADAS)
	Cam_Front = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Cam_Front"));
	Cam_Front->SetupAttachment(RootComponent);
	Cam_Front->bCaptureEveryFrame = false; 

	Cam_Rear = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Cam_Rear"));
	Cam_Rear->SetupAttachment(RootComponent);
	Cam_Rear->bCaptureEveryFrame = false;

	Cam_Left = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Cam_Left"));
	Cam_Left->SetupAttachment(RootComponent);
	Cam_Left->bCaptureEveryFrame = false;

	Cam_Right = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Cam_Right"));
	Cam_Right->SetupAttachment(RootComponent);
	Cam_Right->bCaptureEveryFrame = false;

	Cam_Docking = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Cam_Docking"));
	Cam_Docking->SetupAttachment(RootComponent);
	Cam_Docking->bCaptureEveryFrame = false;
	
	LeftDisplayComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftDisplay"));
	LeftDisplayComp->SetupAttachment(ShipMesh); 
	LeftDisplayComp->SetWidgetSpace(EWidgetSpace::World);
	LeftDisplayComp->SetDrawSize(FVector2D(512.0f, 512.0f));

}

void ASpaceShipPawn::BeginPlay()
{
	Super::BeginPlay();
	if (CFTVSystem)
	{
		CFTVSystem->InitializeCameras(Cam_Front, Cam_Rear, Cam_Left, Cam_Right, Cam_Docking);
	}

}

void ASpaceShipPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CockpitCamera) return;

	// ==========================================
	// A MOLA DE RETORNO DO PESCOÇO (Free Look)
	// ==========================================
	if (CurrentVisionMode != EPilotVisionMode::FreeLook)
	{
		FRotator CurrentHeadRot = CockpitCamera->GetRelativeRotation();
    
		// Se a cabeça não estiver perfeitamente reta (ZeroRotator)
		if (!CurrentHeadRot.Equals(FRotator::ZeroRotator, 0.1f))
		{
			// Volta a cabeça para frente (0,0,0) suavemente
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
			EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Started, this, &ASpaceShipPawn::OnFreeLookStart);
			EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &ASpaceShipPawn::OnFreeLookEnd);
			EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Canceled, this, &ASpaceShipPawn::OnFreeLookEnd);
		}
	}
}


void ASpaceShipPawn::OnThrottle(const FInputActionValue& Value)
{
	float ScrollValue = Value.Get<float>(); // O scroll retorna 1.0 (cima) ou -1.0 (baixo)

	if (MovementComponent)
	{
		MovementComponent->AdjustThrottle(ScrollValue * 0.1f); 
	}
}

void ASpaceShipPawn::Move(const FInputActionValue& Value)
{
	FVector MoveAxisVector = Value.Get<FVector>();

	if (MovementComponent)
	{
		// 1. DESEMPACOTAMENTO:
		// Por padrão no Enhanced Input, o eixo Y (Cima/Baixo) recebe o W e S.
		// O eixo X (Esquerda/Direita) recebe o A e D.
		float ForwardInput = MoveAxisVector.Y; // W/S
		float RightInput   = MoveAxisVector.X; // A/D
		float UpInput      = MoveAxisVector.Z; // Espaço/Ctrl 

		// 2. ENVIANDO PARA O MÚSCULO:
		// Empacotamos na regra clássica da Unreal: X = Frente, Y = Lados, Z = Cima
		MovementComponent->RequestTranslation(FVector(ForwardInput, RightInput, UpInput));
	}
}

// -------------------------------------------------------------
// A MÁQUINA DE ESTADOS (Transições Claras e Livres de Bugs)
// -------------------------------------------------------------
void ASpaceShipPawn::OnToggleYaw(const FInputActionValue& Value)
{
    switch (CurrentRotState)
    {
        case ERotationMode::Locked:      CurrentRotState = ERotationMode::YawControl; break;
        case ERotationMode::RollControl:  CurrentRotState = ERotationMode::YawAndRollControl; break; // Se Roll estava ligado, liga os dois!
        case ERotationMode::YawControl:   CurrentRotState = ERotationMode::Locked; break;     // Se já estava ligado, desliga
        case ERotationMode::YawAndRollControl:  CurrentRotState = ERotationMode::RollControl; break; // Desliga o Yaw, sobra só o Roll
    }
}

void ASpaceShipPawn::OnToggleRoll(const FInputActionValue& Value)
{
    switch (CurrentRotState)
    {
        case ERotationMode::Locked:      CurrentRotState = ERotationMode::RollControl; break;
        case ERotationMode::YawControl:   CurrentRotState = ERotationMode::YawAndRollControl; break;
        case ERotationMode::RollControl:  CurrentRotState = ERotationMode::Locked; break;
        case ERotationMode::YawAndRollControl:  CurrentRotState = ERotationMode::YawControl; break;
    }
}

// -------------------------------------------------------------
// APLICAÇÃO DO INPUT
// -------------------------------------------------------------
void ASpaceShipPawn::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();

	// ==========================================
	// INTERCEPTAÇÃO: O MODO FREE LOOK
	// ==========================================
	if (CurrentVisionMode == EPilotVisionMode::FreeLook)
	{
		FRotator CurrentHeadRot = CockpitCamera->GetRelativeRotation();

		// EIXO Y (PITCH): Se você quiser inverter o eixo da cabeça em relação à nave, 
		// basta trocar o sinal de "+" para "-" na frente do LookAxisVector.Y.
		float NewPitch = CurrentHeadRot.Pitch + (LookAxisVector.Y * LookSensitivity);
		CurrentHeadRot.Pitch = FMath::Clamp(NewPitch, MaxLookDown, MaxLookUp);

		// EIXO X (YAW): Olhar para os lados
		float NewYaw = CurrentHeadRot.Yaw + (LookAxisVector.X * LookSensitivity);
		CurrentHeadRot.Yaw = FMath::Clamp(NewYaw, -MaxLookLeftRight, MaxLookLeftRight);

		// Aplica a rotação apenas na câmera!
		CockpitCamera->SetRelativeRotation(CurrentHeadRot);
        
		return; // ABORTA A FUNÇÃO AQUI! A nave não vai receber o input.
	}
	
    if (MovementComponent)
    {
        float PitchInput = LookAxisVector.Y; 
        float YawInput = 0.0f;
        float RollInput = 0.0f;
    	
        // O Switch lê o estado atual e distribui o valor do Mouse X para o eixo certo!
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
	if (MovementComponent) 
	{
		MovementComponent->SetThrustPowerMode(EThrustPowerMode::Precision);
	}
}

void ASpaceShipPawn::OnPrecisionModeEnd(const FInputActionValue& Value)
{
	if (MovementComponent) 
	{
		MovementComponent->SetThrustPowerMode(EThrustPowerMode::Standard);
	}
}

void ASpaceShipPawn::OnFreeLookStart(const FInputActionValue& Value)
{
	CurrentVisionMode = EPilotVisionMode::FreeLook;
}

void ASpaceShipPawn::OnFreeLookEnd(const FInputActionValue& Value)
{
	if (CurrentVisionMode == EPilotVisionMode::FreeLook)
	{
		CurrentVisionMode = EPilotVisionMode::Standard;
	}
}

