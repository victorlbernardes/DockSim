#include "UI/TelemetryAngularOrientationHUDWidget.h"
#include "Components/SpaceTelemetryComponent.h"

void UTelemetryAngularOrientationHUDWidget::InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp)
{
	CachedTelemetryComp = InTelemetryComp;
}

void UTelemetryAngularOrientationHUDWidget::UpdateContinuousTelemetry()
{
	if (!CachedTelemetryComp) return;

	FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();
		
	if (TXT_Pitch)
	{
		FString Str = FString::Printf(TEXT("PITCH: %5.1f"), Data.Pitch);
		TXT_Pitch->SetText(FText::FromString(Str));
	}
	
	if (TXT_Yaw)
	{
		FString Str = FString::Printf(TEXT("YAW: %5.1f"), Data.Yaw);
		TXT_Yaw->SetText(FText::FromString(Str));
	}
	
	if (TXT_Roll)
	{
		FString Str = FString::Printf(TEXT("ROLL: %5.1f"), Data.Roll);
		TXT_Roll->SetText(FText::FromString(Str));
	}
	
}
