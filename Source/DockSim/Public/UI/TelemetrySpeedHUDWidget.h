#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "TelemetrySpeedHUDWidget.generated.h"

class USpaceTelemetryComponent;
struct FSpaceTelemetryData; 

UCLASS()
class DOCKSIM_API UTelemetrySpeedHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp);
	void UpdateContinuousTelemetry();

protected:
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_FWDPlus;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_FWDMinus;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_LatPlus;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_LatMinus;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_VertPlus;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_VertMinus;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_EngineScale;

private:

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;
	void UpdateDirectionalAxis(float Value, UTextBlock* PositiveWidget, UTextBlock* NegativeWidget, const FString& PositivePrefix, const FString& NegativePrefix);

};
