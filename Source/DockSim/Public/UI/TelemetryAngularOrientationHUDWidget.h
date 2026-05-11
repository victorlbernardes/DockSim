#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TelemetryAngularOrientationHUDWidget.generated.h"

class USpaceTelemetryComponent;
struct FSpaceTelemetryData; 

UCLASS()
class DOCKSIM_API UTelemetryAngularOrientationHUDWidget: public UUserWidget
{
	GENERATED_BODY()
public:
	void InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp);
	void UpdateContinuousTelemetry();

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Pitch;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Yaw;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Roll;


private:

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;


};
