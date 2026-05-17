#include "UI/TelemetryMovementHUDWidget.h"
#include "Components/SpaceTelemetryComponent.h"

void UTelemetryMovementHUDWidget::InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp)
{
	CachedTelemetryComp = InTelemetryComp;
}

void UTelemetryMovementHUDWidget::UpdateDirectionalAxis(float Value, UTextBlock* AxisWidget)
{
	if (!AxisWidget)
	{
		return;
	}

	AxisWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

	FString AxisText;

	if (FMath::IsNearlyZero(Value, 0.05f))
	{
		AxisText = TEXT("0.0 m/s"); 
	}
	else
	{
		AxisText = FString::Printf(TEXT("%5.1f m/s"), Value);
	}

	AxisWidget->SetText(FText::FromString(AxisText));
}

void UTelemetryMovementHUDWidget::UpdateContinuousTelemetry()
{
	if (!CachedTelemetryComp) return;

	FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();
    
    
	UpdateDirectionalAxis(Data.VelForward, TXT_FWDSpeed);
	UpdateDirectionalAxis(Data.VelRight, TXT_LatSpeed);
	UpdateDirectionalAxis(Data.VelUp, TXT_VertSpeed);

	if (TXT_EngineScale)
	{
		FString Str = FString::Printf(TEXT("%5.1f %%"), Data.CurrentEngineScale * 100);
		TXT_EngineScale->SetText(FText::FromString(Str));
	}
	if (TXT_Pitch)
	{
		FString Str = FString::Printf(TEXT("%5.1f"), Data.Pitch);
		TXT_Pitch->SetText(FText::FromString(Str));
	}
	
	if (TXT_Yaw)
	{
		FString Str = FString::Printf(TEXT("%5.1f"), Data.Yaw);
		TXT_Yaw->SetText(FText::FromString(Str));
	}
	
	if (TXT_Roll)
	{
		FString Str = FString::Printf(TEXT("%5.1f"), Data.Roll);
		TXT_Roll->SetText(FText::FromString(Str));
	}
	
	if (TXT_TotalSpeed)
	{
		FString Str = FString::Printf(TEXT("%5.1f m/s"), Data.VelTotal);
		TXT_TotalSpeed->SetText(FText::FromString(Str));
	}
}
