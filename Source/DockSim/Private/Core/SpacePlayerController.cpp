#include "Core/SpacePlayerController.h"
#include "EnhancedInputSubsystems.h"

ASpacePlayerController::ASpacePlayerController()
{
	// Aqui configuraremos o Enhanced Input no futuro.
}

void ASpacePlayerController::BeginPlay()
{
	Super::BeginPlay();
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
