#include "Components/CFTVComponent.h" 
#include "Components/SceneCaptureComponent2D.h"

UCFTVComponent::UCFTVComponent()
{
	PrimaryComponentTick.bCanEverTick = false; 
}

void UCFTVComponent::InitializeCameras(USceneCaptureComponent2D* Front, USceneCaptureComponent2D* Rear, USceneCaptureComponent2D* Left, USceneCaptureComponent2D* Right, USceneCaptureComponent2D* Docking)
{
	// Salva as referências
	Cam_Front = Front;
	Cam_Rear = Rear;
	Cam_Left = Left;
	Cam_Right = Right;
	Cam_Docking = Docking;

	// Garante que o jogo comece com as câmeras certas ligadas
	UpdateCameraPowerStates();
}

void UCFTVComponent::InitializeDockingCamera(USceneCaptureComponent2D* Docking)
{
	Cam_Docking = Docking;
	UpdateDockingCameraPowerStates();
}

void UCFTVComponent::ToggleCameraMode()
{
	if (CurrentCameraMode == ECameraSystemMode::Standby)
	{
		CurrentCameraMode = ECameraSystemMode::DockingMode;
		UE_LOG(LogTemp, Warning, TEXT("CFTV: Monitor Ligado!"));
	}
	else
	{
		CurrentCameraMode = ECameraSystemMode::Standby;
		UE_LOG(LogTemp, Warning, TEXT("CFTV: Monitor Desligado!"));
	}

	UpdateDockingCameraPowerStates();
}

void UCFTVComponent::UpdateCameraPowerStates()
{
	// Medida de segurança: Se as câmeras não foram inicializadas, aborta para não dar Crash.
	if (!Cam_Front || !Cam_Rear || !Cam_Left || !Cam_Right || !Cam_Docking) return;

	if (CurrentCameraMode == ECameraSystemMode::Standby)
	{
		// Modo Voo (Navegação)
		Cam_Front->bCaptureEveryFrame = true;
		Cam_Rear->bCaptureEveryFrame  = true;
		Cam_Left->bCaptureEveryFrame  = true;
		Cam_Right->bCaptureEveryFrame = true;

		Cam_Docking->bCaptureEveryFrame = false;
	}
	else if (CurrentCameraMode == ECameraSystemMode::DockingMode)
	{
		// Modo Acoplamento (Zoom In)
		Cam_Front->bCaptureEveryFrame = false;
		Cam_Rear->bCaptureEveryFrame  = false;
		Cam_Left->bCaptureEveryFrame  = false;
		Cam_Right->bCaptureEveryFrame = false;

		Cam_Docking->bCaptureEveryFrame = true;
	}
}

void UCFTVComponent::UpdateDockingCameraPowerStates()
{
	if (!Cam_Docking) return;

	if (CurrentCameraMode == ECameraSystemMode::Standby)
	{
		Cam_Docking->bCaptureEveryFrame = false;
	}
	else
	{
		Cam_Docking->bCaptureEveryFrame = true;
	}
}