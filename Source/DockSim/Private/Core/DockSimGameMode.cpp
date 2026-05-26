#include "Core/DockSimGameMode.h"
#include "Core/SpacePlayerController.h"
#include "Player/SpaceShipPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SpaceHullComponent.h"
#include "Core/DockSimGameInstance.h"
#include "Data/ShipDataAsset.h"

ADockSimGameMode::ADockSimGameMode()
{
	PlayerControllerClass = ASpacePlayerController::StaticClass();
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

UClass* ADockSimGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (UDockSimGameInstance* GameInst = Cast<UDockSimGameInstance>(GetGameInstance()))
	{
		if (GameInst->SelectedShipData && GameInst->SelectedShipData->ActualShipClass) 
		{
			return GameInst->SelectedShipData->ActualShipClass;
		}
	}

	return ASpaceShipPawn::StaticClass();
}