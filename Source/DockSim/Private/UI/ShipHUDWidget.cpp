#include "UI/ShipHUDWidget.h" 
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Engine/Texture2D.h"
#include "Components/WidgetSwitcher.h"
#include "UI/CameraHUDWidget.h"
#include "UI/TelemetryDockingHUDWidget.h"
#include "UI/TelemetryMovementHUDWidget.h"

void UShipHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		CachedTelemetryComp = PlayerPawn->FindComponentByClass<USpaceTelemetryComponent>();
		if (WBP_TelemetryMovementHUDWidget)
		{
			WBP_TelemetryMovementHUDWidget->InitializeTelemetry(CachedTelemetryComp);
		}
		if (CachedTelemetryComp)
		{
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimer(TelemetryTimerHandle, this, &UShipHUDWidget::UpdateContinuousTelemetry, 0.016f, true);
			}
		}
		
	}
}

void UShipHUDWidget::NativeDestruct()
{
	GetWorld()->GetTimerManager().ClearTimer(TelemetryTimerHandle);
	Super::NativeDestruct();
}

void UShipHUDWidget::UpdateContinuousTelemetry()
{
	if (!CachedTelemetryComp) return;

	if (WBP_TelemetryMovementHUDWidget)
	{
		WBP_TelemetryMovementHUDWidget->UpdateContinuousTelemetry();
	}
}

void UShipHUDWidget::UpdateSignalAlert(ESignalStatus NewStatus)
{
	if (!AlertSwitcher) return;

	switch (NewStatus)
	{
	case ESignalStatus::Lost:
		AlertSwitcher->SetActiveWidgetIndex(0);
		break;

	case ESignalStatus::Acquired:
		AlertSwitcher->SetActiveWidgetIndex(1);
		break;
	}
}

void UShipHUDWidget::SetDockingVideo(UTextureRenderTarget2D* TargetTexture)
{
	if (WBP_DockingPanel)
	{
		WBP_DockingPanel->SetCameraFeed(TargetTexture);
	}
}

void UShipHUDWidget::SetDockingHUDVisible(bool bVisible)
{
	if (WBP_DockingPanel)
	{
		ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
		WBP_DockingPanel->SetVisibility(NewVisibility);
		WBP_TelemetryDockingHUD->SetVisibility(NewVisibility);
	}
}

void UShipHUDWidget::UpdateDockingTelemetry(float Distance, float Speed, float NoseAlign, float RollAlign, const FString& StatusText, FLinearColor TextColor)
{
	if (WBP_TelemetryDockingHUD)
	{
		WBP_TelemetryDockingHUD->UpdateTelemetry(Distance, Speed, NoseAlign, RollAlign, StatusText, TextColor);
	}
}