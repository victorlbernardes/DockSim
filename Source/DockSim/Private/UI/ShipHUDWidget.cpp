#include "UI/ShipHUDWidget.h" 
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Engine/Texture2D.h"
#include "Components/WidgetSwitcher.h"
#include "UI/DockingHUDWidget.h"

void UShipHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// O HUD do capacete volta a usar o padrão Viewport com segurança!
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		// CONFIGURA O TIMER DE TELEMETRIA (Fluxo Contínuo)
		CachedTelemetryComp = PlayerPawn->FindComponentByClass<USpaceTelemetryComponent>();
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

	FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();

	// 1. Descobre a velocidade absoluta
	float SpeedMagnitude = FVector(Data.VelForward, Data.VelRight, Data.VelUp).Size();
	EShipMovementState NewState = (SpeedMagnitude <= 0.05f) ? EShipMovementState::Stabilized : EShipMovementState::Moving;

	// 2. MÁQUINA DE ESTADOS: Feedback Minimalista
	if (NewState != CurrentMovementState)
	{
		FSlateColor TargetColor = (NewState == EShipMovementState::Stabilized) ? FSlateColor(FLinearColor::Green) : FSlateColor(FLinearColor::White);

		if (TXT_VelocityFWD) TXT_VelocityFWD->SetColorAndOpacity(TargetColor);
		if (TXT_VelocityLAT) TXT_VelocityLAT->SetColorAndOpacity(TargetColor);
		if (TXT_VelocityVERT) TXT_VelocityVERT->SetColorAndOpacity(TargetColor);

		if (NewState == EShipMovementState::Stabilized)
		{
			if (IMG_Prograde) IMG_Prograde->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			if (IMG_Prograde) IMG_Prograde->SetVisibility(ESlateVisibility::Visible);
		}
		
		CurrentMovementState = NewState;
	}

	// 3. ATUALIZA OS VETORES NA TELA
	
	// FWD: Forward (Positivo = Frente / Negativo = Ré)
	if (TXT_VelocityFWD)
	{
		FString Str = FString::Printf(TEXT("FWD: %5.1f"), Data.VelForward);
		TXT_VelocityFWD->SetText(FText::FromString(Str));
	}

	// LAT: Lateral (Positivo = Direita / Negativo = Esquerda)
	if (TXT_VelocityLAT)
	{
		FString Str = FString::Printf(TEXT("LAT: %5.1f"), Data.VelRight);
		TXT_VelocityLAT->SetText(FText::FromString(Str));
	}

	// VERT: Vertical (Positivo = Cima / Negativo = Baixo)
	if (TXT_VelocityVERT)
	{
		FString Str = FString::Printf(TEXT("VRT: %5.1f"), Data.VelUp);
		TXT_VelocityVERT->SetText(FText::FromString(Str));
	}
	
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
	
	// CurrentEngineScale: Potência do motor
	if (TXT_EngineScale)
	{
		FString Str = FString::Printf(TEXT("%5.1f %%"), Data.CurrentEngineScale * 100);
		TXT_EngineScale->SetText(FText::FromString(Str));
	}
	
	// B. Ícone Prograde / Retrograde (Só calcula se estiver se movendo)
	if (IMG_Prograde && CurrentMovementState == EShipMovementState::Moving)
	{
		if (Data.VelForward < 0) 
		{
			if (Tex_Retrograde) IMG_Prograde->SetBrushFromTexture(Tex_Retrograde);
		}
		else 
		{
			if (Tex_Prograde) IMG_Prograde->SetBrushFromTexture(Tex_Prograde);
		}

		const float PixelPerMeter = 15.0f; 
		float TranslationX = FMath::Clamp(Data.VelRight * PixelPerMeter, -400.0f, 400.0f);
		float TranslationY = FMath::Clamp(-Data.VelUp * PixelPerMeter, -400.0f, 400.0f);

		IMG_Prograde->SetRenderTranslation(FVector2D(TranslationX, TranslationY));
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
	if (WBP_DockingPanel)
	{
		WBP_DockingPanel->UpdateTelemetry(Distance, Speed, NoseAlign, RollAlign, StatusText, TextColor);
	}
}