#include "UI/ShipHUDWidget.h" 
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Engine/Texture2D.h"
#include "Components/WidgetSwitcher.h"
#include "UI/CameraHUDWidget.h"
#include "UI/TelemetryDockingHUDWidget.h"
#include "UI/TelemetrySpeedHUDWidget.h"

void UShipHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// O HUD do capacete volta a usar o padrão Viewport com segurança!
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		// CONFIGURA O TIMER DE TELEMETRIA (Fluxo Contínuo)
		CachedTelemetryComp = PlayerPawn->FindComponentByClass<USpaceTelemetryComponent>();
		if (WBP_TelemetrySpeedHUDWidget)
		{
			WBP_TelemetrySpeedHUDWidget->InitializeTelemetry(CachedTelemetryComp);
		}
		if (CachedTelemetryComp)
		{
			// Em vez de NativeTick, chamamos a função a cada 0.1 segundos (10 FPS)
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

	if (WBP_TelemetrySpeedHUDWidget)
	{
		WBP_TelemetrySpeedHUDWidget->UpdateContinuousTelemetry();
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
	// Delega a responsabilidade para o submódulo
	if (WBP_DockingPanel)
	{
		WBP_DockingPanel->SetCameraFeed(TargetTexture);
	}
}

void UShipHUDWidget::SetDockingHUDVisible(bool bVisible)
{
	// Esconde ou mostra o painel inteiro de docking
	if (WBP_DockingPanel)
	{
		ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden;
		WBP_DockingPanel->SetVisibility(NewVisibility);
	}
}

void UShipHUDWidget::UpdateDockingTelemetry(float Distance, float Speed, float NoseAlign, float RollAlign, const FString& StatusText, FLinearColor TextColor)
{
	// Repassa os dados para o widget especializado tratar as cores e textos
	if (WBP_TelemetryDockingHUD)
	{
		WBP_TelemetryDockingHUD->UpdateTelemetry(Distance, Speed, NoseAlign, RollAlign, StatusText, TextColor);
	}
}