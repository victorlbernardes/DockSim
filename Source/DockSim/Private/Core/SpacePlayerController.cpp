#include "Core/SpacePlayerController.h"
#include "EnhancedInputSubsystems.h"

ASpacePlayerController::ASpacePlayerController()
{
	// Aqui configuraremos o Enhanced Input no futuro.
}

void ASpacePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HUDClass)
	{
		MainHUDInstance = CreateWidget<UShipHUDWidget>(this, HUDClass);

		if (MainHUDInstance)
		{
			MainHUDInstance->AddToViewport();

			UE_LOG(LogTemp, Warning, TEXT("HUD criado no PlayerController"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HUDClass não está setado!"));
	}
}

void ASpacePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ShipMappingContext)
		{
			Subsystem->ClearAllMappings(); // opcional (bom pra evitar conflito)
			Subsystem->AddMappingContext(ShipMappingContext, 0);
		}
	}
}

void ASpacePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void ASpacePlayerController::UpdateSignalAlert(ESignalStatus NewStatus)
{
	if (MainHUDInstance)
	{
		MainHUDInstance->UpdateSignalAlert(NewStatus);
	}
}
