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
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow|UI")
	void ShowGameOverScreen();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Flow|UI")
	void ShowVictoryScreen();
	
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	bool bStartInCinematic = true;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnShipDestroyed();
	
	UFUNCTION()
	void OnMissionVictory();
	
	UFUNCTION()
	void OnIntroCinematicFinished();
	
};
