#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DockSimGameMode.generated.h"

UCLASS()
class DOCKSIM_API ADockSimGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ADockSimGameMode();
	// Essa macro mágica diz: "O C++ vai chamar essa função, mas a LÓGICA dela será feita no Blueprint!"
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow|UI")
	void ShowGameOverScreen();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow|UI")
	void ShowVictoryScreen();
	
protected:
	virtual void BeginPlay() override;

	// A função do GameMode que vai escutar o Delegate do Casco
	UFUNCTION()
	void OnShipDestroyed();
};
