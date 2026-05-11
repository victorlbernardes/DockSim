#include "UI/CameraHUDWidget.h"
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h" 

void UCameraHUDWidget::SetCameraFeed(UTextureRenderTarget2D* CameraTexture)
{
	if (IMG_CameraFeed && CameraTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(CameraTexture);

		IMG_CameraFeed->SetBrush(Brush);
	}
}