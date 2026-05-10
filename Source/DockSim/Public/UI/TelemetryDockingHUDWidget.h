#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "TelemetryDockingHUDWidget.generated.h"

class UTextBlock;
class UImage; 
UCLASS()
class UTelemetryDockingHUDWidget: public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateTelemetry(float DistanceMeters, float SpeedMPS, float AlignmentNoseAlignmentPct, float AlignmentRollPct, FString CurrentStatusMsg, FLinearColor StatusColor);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Distance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Speed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Status;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_NoseAlignmentAlignment;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_RollAlignment;

};

