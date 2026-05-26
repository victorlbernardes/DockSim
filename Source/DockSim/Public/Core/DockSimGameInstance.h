#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DockSimGameInstance.generated.h"

class UShipDataAsset; // Conhece o nosso Data Asset de Naves

UCLASS()
class DOCKSIM_API UDockSimGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Game Data")
	TObjectPtr<UShipDataAsset> SelectedShipData;

};