#include "UI/ShipHUDWidget.h" 
#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Engine/Texture2D.h"

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
			GetWorld()->GetTimerManager().SetTimer(TelemetryTimerHandle, this, &UShipHUDWidget::UpdateContinuousTelemetry, 0.1f, true);
		}
		
		if (IMG_PitchLadder)
		{
			PitchLadderMID = IMG_PitchLadder->GetDynamicMaterial();
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
		if (NewState == EShipMovementState::Stabilized)
		{
			// NAVE PAROU: Texto Verde e esconde o Prograde
			if (TXT_Velocity) TXT_Velocity->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
			if (IMG_Prograde) IMG_Prograde->SetVisibility(ESlateVisibility::Hidden);
		}
		else if (NewState == EShipMovementState::Moving)
		{
			// NAVE MOVENDO: Texto Branco e volta a mostrar o Prograde
			if (TXT_Velocity) TXT_Velocity->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			if (IMG_Prograde) IMG_Prograde->SetVisibility(ESlateVisibility::Visible);
		}
		CurrentMovementState = NewState;
	}

	// 3. ATUALIZA OS DADOS TODO FRAME
	
	// A. Texto da Velocidade
	if (TXT_Velocity)
	{
		FString VelString = FString::Printf(TEXT("%.1f m/s"), Data.VelForward);
		TXT_Velocity->SetText(FText::FromString(VelString));
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
	
	// C. Pitch Ladder
	if (PitchLadderMID)
	{
		float NormalizedRoll = -Data.Roll / 360.0f; 
		float NormalizedPitch = Data.Pitch / 90.0f; 

		PitchLadderMID->SetScalarParameterValue(FName("RollAngle"), NormalizedRoll);
		PitchLadderMID->SetScalarParameterValue(FName("PitchOffset"), NormalizedPitch);
	}
}