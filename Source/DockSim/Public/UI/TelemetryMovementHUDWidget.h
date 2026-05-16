#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TelemetryMovementHUDWidget.generated.h"

class USpaceTelemetryComponent;
struct FSpaceTelemetryData; 

UCLASS()
class DOCKSIM_API UTelemetryMovementHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp);
	void UpdateContinuousTelemetry();

protected:
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_FWDSpeed;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_LatSpeed;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_VertSpeed;
	
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UTextBlock> TXT_TotalSpeed;
		
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_EngineScale;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Pitch;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Yaw;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Roll;

private:

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;
	void UpdateDirectionalAxis(float Value, UTextBlock* AxisWidget);

};
