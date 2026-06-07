#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DockSimGameInstance.generated.h"

class UShipDataAsset;

UCLASS()
class DOCKSIM_API UDockSimGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Game Data")
	TObjectPtr<UShipDataAsset> SelectedShipData;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Game Data")
	FName SelectedStageData;

};