#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CameraHUDWidget.generated.h"

class UImage; 


UCLASS()
class DOCKSIM_API UCameraHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetCameraFeed(UTextureRenderTarget2D* CameraTexture);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IMG_CameraFeed;
};