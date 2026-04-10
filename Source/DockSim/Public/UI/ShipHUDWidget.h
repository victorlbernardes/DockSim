#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/SpaceMovementComponent.h"
#include "ShipHUDWidget.generated.h"

class USpaceTelemetryComponent;

UCLASS()
class DOCKSIM_API UShipHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Flight UI")
	void OnTelemetryUpdated(const FSpaceTelemetryData& TelemetryData);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	// ==========================================
	// COMPONENTES VISUAIS (Frontend - BindWidget)
	// ==========================================
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Velocity;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> PB_Fuel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_ThrustMode;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* IMG_Prograde;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* IMG_PitchLadder;

	// Evento para o Artista chamar animações (ex: Tela piscando em vermelho)
	UFUNCTION(BlueprintImplementableEvent, Category = "Flight UI")
	void OnFuelEmptyAlarm();

private:
	// ==========================================
	// RECEPTORES DE EVENTOS (Delegates)
	// ==========================================
	
	UFUNCTION()
	void HandleThrustModeChanged(EThrustPowerMode NewMode);

	UFUNCTION()
	void HandleFuelEmpty();

	// ==========================================
	// ROTINAS CONTÍNUAS (Polling a 10Hz)
	// ==========================================
	
	UFUNCTION()
	void UpdateContinuousTelemetry();

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;
	
	UPROPERTY()
	UMaterialInstanceDynamic* PitchLadderMID;

	FTimerHandle TelemetryTimerHandle;
};
