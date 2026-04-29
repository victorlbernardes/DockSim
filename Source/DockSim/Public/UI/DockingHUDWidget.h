#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DockingHUDWidget.generated.h"

class UTextBlock;
class UImage; // <- ADICIONE ISTO
class UTextureRenderTarget2D; // <- ADICIONE ISTO

UCLASS()
class DOCKSIM_API UDockingHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateTelemetry(float DistanceMeters, float SpeedMPS, float AlignmentPct, FString CurrentStatusMsg, FLinearColor StatusColor);
	
	// A função que a Nave usará para injetar a imagem da câmera
	void SetCameraFeed(UTextureRenderTarget2D* CameraTexture);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Distance;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Speed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Status;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Alignment;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IMG_CameraFeed;
};