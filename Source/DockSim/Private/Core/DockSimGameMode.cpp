#include "Core/DockSimGameMode.h"
#include "Core/SpacePlayerController.h"
#include "Player/SpaceShipPawn.h"

ADockSimGameMode::ADockSimGameMode()
{
	PlayerControllerClass = ASpacePlayerController::StaticClass();
	DefaultPawnClass = ASpaceShipPawn::StaticClass();
}