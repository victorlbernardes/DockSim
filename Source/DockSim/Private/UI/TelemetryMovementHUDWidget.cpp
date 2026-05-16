#include "UI/TelemetryMovementHUDWidget.h"
#include "Components/SpaceTelemetryComponent.h"

void UTelemetryMovementHUDWidget::InitializeTelemetry(USpaceTelemetryComponent* InTelemetryComp)
{
	CachedTelemetryComp = InTelemetryComp;
}

void UTelemetryMovementHUDWidget::UpdateDirectionalAxis(float Value, UTextBlock* AxisWidget)
{
	// Proteção contra ponteiros nulos (sempre necessário!)
	if (!AxisWidget)
	{
		return;
	}

	// 1. GARANTIA DE VISIBILIDADE
	AxisWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

	FString AxisText;

	// 2. MÁQUINA DE ESTADOS DO TEXTO (Simplificada)
	if (FMath::IsNearlyZero(Value, 0.05f))
	{
		// Se estiver parado (ou quase), forçamos 0.0 para não exibir "-0.0" ou sujeiras matemáticas
		AxisText = TEXT("0.0 m/s"); 
	}
	else
	{
		// O C++ injeta o sinal de negativo automaticamente se Value for < 0.
		// Valores positivos ficam sem sinal.
		AxisText = FString::Printf(TEXT("%5.1f m/s"), Value);
	}

	// 3. APLICAÇÃO DO TEXTO
	AxisWidget->SetText(FText::FromString(AxisText));
}

void UTelemetryMovementHUDWidget::UpdateContinuousTelemetry()
{
	if (!CachedTelemetryComp) return;

	FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();
    
	// ATUALIZAÇÃO CONTÍNUA: Roda independente do estado mudar ou não
    
	UpdateDirectionalAxis(Data.VelForward, TXT_FWDSpeed);
	UpdateDirectionalAxis(Data.VelRight, TXT_LatSpeed);
	UpdateDirectionalAxis(Data.VelUp, TXT_VertSpeed);

	// CurrentEngineScale: Potência do motor
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
