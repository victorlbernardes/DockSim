#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/SpaceShipPawn.h" 
#include "SpacePlayerController.generated.h"

UENUM(BlueprintType)
enum class EMissionState : uint8
{
    Cinematic   UMETA(DisplayName = "Cinematic"),
    Playing     UMETA(DisplayName = "Playing"),
    Paused      UMETA(DisplayName = "Paused"),
    Victory     UMETA(DisplayName = "Victory"),
    GameOver    UMETA(DisplayName = "Game Over")
};

class UShipHUDWidget;
class UMissionMenuWidget;
class UInputMappingContext;

UCLASS()
class DOCKSIM_API ASpacePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASpacePlayerController();

    UFUNCTION(BlueprintCallable, Category = "Space|GameLoop")
    void SetMissionState(EMissionState NewState);

    UFUNCTION(BlueprintCallable, Category = "Space|GameLoop")
    void TogglePauseMenu();

    UFUNCTION(BlueprintCallable, Category = "Space|UI")
    void UpdateSignalAlert(ESignalStatus NewStatus);

    UFUNCTION(BlueprintPure, Category = "Space|GameLoop")
    EMissionState GetCurrentMissionState() const { return CurrentMissionState; }
    
    
    UShipHUDWidget* GetHUDWidget() const { return MainHUDInstance; }

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditDefaultsOnly, Category = "Space|Input")
    TObjectPtr<UInputMappingContext> ShipMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Space|UI")
    TSubclassOf<UShipHUDWidget> HUDClass;

    UPROPERTY(EditDefaultsOnly, Category = "Space|UI")
    TSubclassOf<UMissionMenuWidget> MissionMenuClass;

private:
    UPROPERTY()
    TObjectPtr<UShipHUDWidget> MainHUDInstance;

    UPROPERTY()
    TObjectPtr<UMissionMenuWidget> ActiveMenuInstance;

    EMissionState CurrentMissionState = EMissionState::Paused;

    void HandleStateChange(EMissionState NewState);
};