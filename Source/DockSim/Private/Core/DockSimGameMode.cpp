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

	// 1. Encontra a nave do jogador no mapa
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

	// 1. Pega o "controle" do jogador para soltar o mouse na tela
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->SetShowMouseCursor(true); // Faz a setinha do Windows aparecer
		PC->SetInputMode(FInputModeUIOnly()); // Trava a câmera e os controles de voo, o mouse agora só clica em botões
	}

	// 2. Chama a função que desenha a UI (que você vai criar no Blueprint)
	ShowGameOverScreen();
}

void ADockSimGameMode::OnMissionVictory() {
	// 1. Pausa e libera o mouse (Reutilizando sua lógica de derrota)[cite: 8]
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC) {
		PC->SetShowMouseCursor(true);
		PC->SetInputMode(FInputModeUIOnly());
	}
    
	// 2. Chama o evento visual que será feito no Blueprint
	ShowVictoryScreen();
}