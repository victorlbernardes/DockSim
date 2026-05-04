#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Player/SpaceShipPawn.h" // Para acessar ESignalStatus se necessário
#include "SpacePlayerController.generated.h"

/** * Estados da Missão para controle de fluxo sem booleans.
 */
UENUM(BlueprintType)
enum class EMissionState : uint8
{
    Playing     UMETA(DisplayName = "Jogando"),
    Paused      UMETA(DisplayName = "Pausado"),
    Victory     UMETA(DisplayName = "Vitória"),
    GameOver    UMETA(DisplayName = "Game Over")
};

// Forward Declarations
class UShipHUDWidget;
class UMissionMenuWidget;
class UInputMappingContext;

UCLASS()
class DOCKSIM_API ASpacePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ASpacePlayerController();

    /** Altera o estado da missão e gerencia a UI/Pausa */
    UFUNCTION(BlueprintCallable, Category = "Space|GameLoop")
    void SetMissionState(EMissionState NewState);

    /** Alterna o menu de pausa (chamado pelo ESC na Pawn) */
    UFUNCTION(BlueprintCallable, Category = "Space|GameLoop")
    void TogglePauseMenu();

    /** Atualiza alertas de sinal vindos da estação espacial */
    UFUNCTION(BlueprintCallable, Category = "Space|UI")
    void UpdateSignalAlert(ESignalStatus NewStatus);

    UFUNCTION(BlueprintPure, Category = "Space|GameLoop")
    EMissionState GetCurrentMissionState() const { return CurrentMissionState; }

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

    // --- CONFIGURAÇÕES DE INPUT ---
    UPROPERTY(EditDefaultsOnly, Category = "Space|Input")
    TObjectPtr<UInputMappingContext> ShipMappingContext;

    // --- CONFIGURAÇÕES DE UI ---
    UPROPERTY(EditDefaultsOnly, Category = "Space|UI")
    TSubclassOf<UShipHUDWidget> HUDClass;

    UPROPERTY(EditDefaultsOnly, Category = "Space|UI")
    TSubclassOf<UMissionMenuWidget> MissionMenuClass;

private:
    /** Instância do HUD principal (Telemetria) */
    UPROPERTY()
    TObjectPtr<UShipHUDWidget> MainHUDInstance;

    /** Instância ativa do Menu de Missão */
    UPROPERTY()
    TObjectPtr<UMissionMenuWidget> ActiveMenuInstance;

    /** Estado atual da máquina de estados */
    EMissionState CurrentMissionState = EMissionState::Paused;

    /** Lógica interna de troca de estado (Mouse, Pausa, Visibilidade) */
    void HandleStateChange(EMissionState NewState);
};