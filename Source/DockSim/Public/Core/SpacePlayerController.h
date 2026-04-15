#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/ShipHUDWidget.h"
#include "SpacePlayerController.generated.h"

class UInputMappingContext;
UCLASS()
class DOCKSIM_API ASpacePlayerController: public APlayerController 
{
	GENERATED_BODY()
	
public:
	ASpacePlayerController();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player Controller")
	TObjectPtr<UInputMappingContext> ShipMappingContext;
	void UpdateSignalAlert(ESignalStatus NewStatus);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UShipHUDWidget> HUDClass;

	UPROPERTY()
	TObjectPtr<UShipHUDWidget> MainHUDInstance;
	
	
};
