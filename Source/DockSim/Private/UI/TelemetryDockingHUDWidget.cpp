#include "UI/TelemetryDockingHUDWidget.h"

void UTelemetryDockingHUDWidget::UpdateTelemetry(float DistanceMeters, float SpeedMPS, float NoseAlignmentYawPct, float AlignmentRollPct, FString CurrentStatusMsg, FLinearColor StatusColor)
{
	if (TXT_Distance)
	{
		FString DistString = FString::Printf(TEXT("%3.1f m"), DistanceMeters);
		TXT_Distance->SetText(FText::FromString(DistString));
	}

	if (TXT_Speed)
	{
		FString SpeedString = FString::Printf(TEXT("%4.1f m/s"), SpeedMPS);
		TXT_Speed->SetText(FText::FromString(SpeedString));

		if (SpeedMPS > 2.0f)
		{
			TXT_Speed->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else if (SpeedMPS > 0.5f)
		{
			TXT_Speed->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
		else
		{
 			TXT_Speed->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
	}
	
	if (TXT_NoseAlignmentAlignment)
	{
		FString AlignString = FString::Printf(TEXT("Pitch Allign: %04.1f%%"), NoseAlignmentYawPct);
		TXT_NoseAlignmentAlignment->SetText(FText::FromString(AlignString));

		if (NoseAlignmentYawPct >= 95.0f)
		{
			TXT_NoseAlignmentAlignment->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
		else
		{
			TXT_NoseAlignmentAlignment->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
	}
	
	if (TXT_RollAlignment)
	{
		FString AlignString = FString::Printf(TEXT("Roll Allign: %04.1f%%"), AlignmentRollPct);
		TXT_RollAlignment->SetText(FText::FromString(AlignString));

		if (AlignmentRollPct >= 95.0f)
		{
			TXT_RollAlignment->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
		else
		{
			TXT_RollAlignment->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
	}
	
	if (TXT_Status)
	{
		TXT_Status->SetText(FText::FromString(CurrentStatusMsg));
		TXT_Status->SetColorAndOpacity(FSlateColor(StatusColor));
	}
}