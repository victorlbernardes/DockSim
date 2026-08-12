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
	TObjectPtr<UStaticMesh> DisplayMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TSubclassOf<class ASpaceShipPawn> ActualShipClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText ShipName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText DockingType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText ShipMass;
	
};