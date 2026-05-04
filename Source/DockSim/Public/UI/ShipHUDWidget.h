#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Player/SpaceShipPawn.h"
#include "ShipHUDWidget.generated.h"

class USpaceTelemetryComponent;
struct FSpaceTelemetryData; 

UENUM(BlueprintType)
enum class EShipMovementState : uint8
{
	Moving      UMETA(DisplayName = "Em Movimento (Derivando)"),
	Stabilized  UMETA(DisplayName = "Inércia Zero (Travado)")
};

UCLASS()
class DOCKSIM_API UShipHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Flight UI")
	void OnTelemetryUpdated(const FSpaceTelemetryData& TelemetryData);
	UFUNCTION(BlueprintCallable, Category = "Flight UI|Docking")
	void UpdateSignalAlert(ESignalStatus NewStatus);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_VelocityFWD;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_VelocityLAT;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_VelocityVERT;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Pitch;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Yaw;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Roll;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_EngineScale;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IMG_Prograde;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flight UI|Icons")
	TObjectPtr<class UTexture2D> Tex_Prograde;

	UPROPERTY(EditDefaultsOnly, Category = "Flight UI|Icons")
	TObjectPtr<class UTexture2D> Tex_Retrograde;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> AlertSwitcher;

private:
	
	UFUNCTION()
	void UpdateContinuousTelemetry();

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;

	FTimerHandle TelemetryTimerHandle;
	EShipMovementState CurrentMovementState = EShipMovementState::Stabilized;
};