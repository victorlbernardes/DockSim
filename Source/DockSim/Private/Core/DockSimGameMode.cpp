#include "Core/DockSimGameMode.h"
#include "Core/SpacePlayerController.h"
#include "Player/SpaceShipPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SpaceHullComponent.h"

ADockSimGameMode::ADockSimGameMode()
{
	PlayerControllerClass = ASpacePlayerController::StaticClass();
	DefaultPawnClass = ASpaceShipPawn::StaticClass();
}

void ADockSimGameMode::BeginPlay()
{
	Super::BeginPlay();

	ASpaceShipPawn* PlayerShip = Cast<ASpaceShipPawn>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (PlayerShip && PlayerShip->GetHullComponent())
	{
		PlayerShip->GetHullComponent()->OnHullCritical.AddDynamic(this, &ADockSimGameMode::OnShipDestroyed);
		
		PlayerShip->OnDockingCompleted.AddDynamic(this, &ADockSimGameMode::OnMissionVictory);
	}
}

void ADockSimGameMode::OnShipDestroyed()
{
	UE_LOG(LogTemp, Error, TEXT("GAMEMODE: Nave destruída. Invocando Game Over."));

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetShowMouseCursor(true); 
		PC->SetInputMode(FInputModeUIOnly()); 
	}

	ShowGameOverScreen();
}

void ADockSimGameMode::OnMissionVictory() {
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC) {
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
    
	ShowVictoryScreen();
}