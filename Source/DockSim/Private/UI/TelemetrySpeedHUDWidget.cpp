#include "UI/TelemetrySpeedHUDWidget.h"
#include "Components/SpaceTelemetryComponent.h"

void UTelemetrySpeedHUDWidget::InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp)
{
	CachedTelemetryComp = InTelemetryComp;
}

void UTelemetrySpeedHUDWidget::UpdateDirectionalAxis(
	float Value,
	UTextBlock* PositiveWidget,
	UTextBlock* NegativeWidget,
	const FString& PositivePrefix,
	const FString& NegativePrefix)
{
	if (!PositiveWidget || !NegativeWidget)
	{
		return;
	}

	// 1. GARANTIA DE VISIBILIDADE
	// Como a nova regra é "mostrar tudo sempre", já cravamos a visibilidade 
	// no topo da função. Não precisamos mais esconder (Collapsed) ninguém.
	PositiveWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	NegativeWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

	const float AbsValue = FMath::Abs(Value);
	
	FString PosText;
	FString NegText;

	// 2. MÁQUINA DE ESTADOS DO TEXTO
	if (FMath::IsNearlyZero(Value, 0.05f))
	{
		// Se estiver parado (ou quase), ambos mostram 0.0
		PosText = FString::Printf(TEXT("%s   0.0"), *PositivePrefix);
		NegText = FString::Printf(TEXT("%s   0.0"), *NegativePrefix);
	}
	else if (Value > 0.f)
	{
		// Movimento Positivo (Ex: Indo para Cima)
		// Positivo mostra a velocidade real, Negativo crava em 0.0
		PosText = FString::Printf(TEXT("%s %5.1f"), *PositivePrefix, AbsValue);
		NegText = FString::Printf(TEXT("%s   0.0"), *NegativePrefix);
	}
	else
	{
		// Movimento Negativo (Ex: Indo para Baixo)
		// Positivo crava em 0.0, Negativo mostra a velocidade real (Absoluta)
		PosText = FString::Printf(TEXT("%s   0.0"), *PositivePrefix);
		NegText = FString::Printf(TEXT("%s %5.1f"), *NegativePrefix, AbsValue);
	}

	// 3. APLICAÇÃO DOS TEXTOS
	PositiveWidget->SetText(FText::FromString(PosText));
	NegativeWidget->SetText(FText::FromString(NegText));
}

void UTelemetrySpeedHUDWidget::UpdateContinuousTelemetry()
{
	if (!CachedTelemetryComp) return;

	FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();
	
	// ATUALIZAÇÃO CONTÍNUA: Roda independente do estado mudar ou não
	
	UpdateDirectionalAxis(
		Data.VelForward,
		TXT_FWDPlus,
		TXT_FWDMinus,
		TEXT("FWD"),
		TEXT("REV"));

	UpdateDirectionalAxis(
		Data.VelRight,
		TXT_LatPlus,
		TXT_LatMinus,
		TEXT("RIGHT"),
		TEXT("LEFT"));

	UpdateDirectionalAxis(
		Data.VelUp,
		TXT_VertPlus,
		TXT_VertMinus,
		TEXT("UP"),
		TEXT("DOWN"));

	// CurrentEngineScale: Potência do motor
	if (TXT_EngineScale)
	{
		FString Str = FString::Printf(TEXT("%5.1f %%"), Data.CurrentEngineScale * 100);
		TXT_EngineScale->SetText(FText::FromString(Str));
	}
	
}
