#include "UI/ResourceWidget.h"

#include "Components/ProgressBar.h"
#include "Components/SpaceResourceComponent.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "Player/SpaceShipPawn.h"

void UResourceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		CachedResourceComp = PlayerPawn->FindComponentByClass<USpaceResourceComponent>();
        
		if (CachedResourceComp.IsValid())
		{
			CachedResourceComp->OnFuelStatusChanged.AddDynamic(this, &UResourceWidget::HandleFuelStatusChanged);
		}
		
		CachedTelemetryComp = PlayerPawn->FindComponentByClass<USpaceTelemetryComponent>();
	}
}

void UResourceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CachedResourceComp.IsValid())
	{
		if (PB_Fuel && CachedResourceComp.IsValid())
		{
			PB_Fuel->SetPercent(CachedResourceComp->GetFuelPercent());
		}
		if (TXT_FuelPercent)
		{
			FString FuelStr = FString::Printf(TEXT("%.1f %%"), CachedResourceComp->GetFuelPercent() * 100.0f);
			TXT_FuelPercent->SetText(FText::FromString(FuelStr));
		}
	}
	if (CachedTelemetryComp.IsValid())
	{
		FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();
		if (TXT_ThrustMode)
		{
			FString ModeString = (Data.ThrustMode == EThrustPowerMode::Precision) ? TEXT("PRECISION") : TEXT("STANDARD");
			TXT_ThrustMode->SetText(FText::FromString(ModeString));
			
			FLinearColor TextColor = (Data.ThrustMode == EThrustPowerMode::Precision) ? FLinearColor::Green : FColor::FromHex("#FFB347");
			TXT_ThrustMode->SetColorAndOpacity(FSlateColor(TextColor));
		}
	}
	if (TXT_Headlights)
	{
		ASpaceShipPawn* ShipPawn = Cast<ASpaceShipPawn>(PlayerPawn);

		if (ShipPawn)
		{
			const bool bHeadlightsOn = ShipPawn->GetHeadlight();
			TXT_Headlights->SetText(
				bHeadlightsOn
				? FText::FromString("ON")
				: FText::FromString("OFF")
			);
			TXT_Headlights->SetColorAndOpacity(
				bHeadlightsOn
				? FSlateColor(FLinearColor::Green)
				: FSlateColor(FLinearColor(FColor::FromHex("#FFB347")))
			);
		}
	}
}

void UResourceWidget::HandleFuelStatusChanged(EFuelStatus NewStatus)
{
	if (!PB_Fuel) return;

	switch (NewStatus)
	{
		case EFuelStatus::Normal:
			PB_Fuel->SetFillColorAndOpacity(FLinearColor::Green);
			TXT_FuelPercent->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			break;
		case EFuelStatus::LowFuel:
			PB_Fuel->SetFillColorAndOpacity(FLinearColor::Red);
			TXT_FuelPercent->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
			break;
		case EFuelStatus::Empty:
			PB_Fuel->SetFillColorAndOpacity(FLinearColor::Black);
			TXT_FuelPercent->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
			break;
		default:
			PB_Fuel->SetFillColorAndOpacity(FLinearColor::Green);
			TXT_FuelPercent->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		break;
	}
	
}