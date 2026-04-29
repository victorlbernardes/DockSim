#include "UI/DockingHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h" 

void UDockingHUDWidget::UpdateTelemetry(float DistanceMeters, float SpeedMPS, float AlignmentPct, FString CurrentStatusMsg, FLinearColor StatusColor)
{
	if (TXT_Distance)
	{
		// Formata o número para ter sempre zeros à esquerda e 1 casa decimal (Ex: 014.5)
		FString DistString = FString::Printf(TEXT("DST: %3.1f m"), DistanceMeters);
		TXT_Distance->SetText(FText::FromString(DistString));
	}

	if (TXT_Speed)
	{
		FString SpeedString = FString::Printf(TEXT("SPD: %4.1f m/s"), SpeedMPS);
		TXT_Speed->SetText(FText::FromString(SpeedString));

		// Lógica de Tensão Visual: Se passar de 2.0 m/s (perigo), fica vermelho
		if (SpeedMPS > 2.0f)
		{
			TXT_Speed->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		}
		else
		{
			TXT_Speed->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
	}
	
	if (TXT_Alignment)
	{
		// Exibe a porcentagem (Ex: 98.4%)
		FString AlignString = FString::Printf(TEXT("ALINHAMENTO: %04.1f%%"), AlignmentPct);
		TXT_Alignment->SetText(FText::FromString(AlignString));

		// Se o alinhamento estiver acima de 95%, brilha em verde, senão amarelo/laranja
		if (AlignmentPct >= 95.0f)
		{
			TXT_Alignment->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
		}
		else
		{
			TXT_Alignment->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
		}
	}
	
	if (TXT_Status)
	{
		TXT_Status->SetText(FText::FromString(CurrentStatusMsg));
		TXT_Status->SetColorAndOpacity(FSlateColor(StatusColor));
	}
}

void UDockingHUDWidget::SetCameraFeed(UTextureRenderTarget2D* CameraTexture)
{
	// Se a imagem no painel existir e o vídeo da nave for válido...
	if (IMG_CameraFeed && CameraTexture)
	{
		// Transforma o vídeo em um "Brush" e pinta a imagem do Blueprint
		IMG_CameraFeed->SetBrushResourceObject(CameraTexture);
	}
}