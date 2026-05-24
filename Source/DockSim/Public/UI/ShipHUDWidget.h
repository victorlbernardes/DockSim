#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Player/SpaceShipPawn.h"
#include "ShipHUDWidget.generated.h"

class USpaceTelemetryComponent;
class UCameraHUDWidget;
class UTelemetryDockingHUDWidget;
class UTelemetryMovementHUDWidget;
struct FSpaceTelemetryData; 

UCLASS()
class DOCKSIM_API UShipHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Flight UI")
	void OnTelemetryUpdated(const FSpaceTelemetryData& TelemetryData);
	UFUNCTION(BlueprintCallable, Category = "Flight UI|Docking")
	void UpdateSignalAlert(ESignalStatus NewStatus);
	
	UFUNCTION(BlueprintCallable, Category = "Docking")
	void SetDockingVideo(UTextureRenderTarget2D* TargetTexture);
	
	UFUNCTION(BlueprintCallable, Category = "Docking")
	void SetDockingHUDVisible(bool bVisible);
	
	UFUNCTION(BlueprintCallable, Category = "Docking")
	void UpdateDockingTelemetry(float Distance, float Speed, float NoseAlign, float RollAlign, const FString& StatusText, FLinearColor TextColor);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> AlertSwitcher;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCameraHUDWidget> WBP_DockingPanel;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTelemetryDockingHUDWidget> WBP_TelemetryDockingHUD;
		
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTelemetryMovementHUDWidget> WBP_TelemetryMovementHUDWidget;

private:
	
	UFUNCTION()
	void UpdateContinuousTelemetry();

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;

	FTimerHandle TelemetryTimerHandle;
};