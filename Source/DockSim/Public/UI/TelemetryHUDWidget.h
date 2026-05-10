#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "TelemetryHUDWidget.generated.h"

class USpaceTelemetryComponent;
struct FSpaceTelemetryData; 

UENUM(BlueprintType)
enum class EShipMovementState : uint8
{
	Moving      UMETA(DisplayName = "Em Movimento (Derivando)"),
	Stabilized  UMETA(DisplayName = "Inércia Zero (Travado)")
};

UCLASS()
class DOCKSIM_API UTelemetryHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp);
	void UpdateContinuousTelemetry();

protected:
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

private:

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;

	EShipMovementState CurrentMovementState = EShipMovementState::Stabilized;
};
