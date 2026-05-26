#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShipDataAsset.generated.h"

UCLASS(BlueprintType)
class DOCKSIM_API UShipDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMesh> DisplayMesh; // Modelo leve para o Menu

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<class ASpaceShipPawn> ActualShipClass; // A classe real do gameplay

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText ShipName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText DifficultyLevel;

	// Status para desenhar barrinhas na UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ThrustStat; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HandlingStat;
};