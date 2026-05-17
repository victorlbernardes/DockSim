#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CFTVComponent.generated.h"

class USceneCaptureComponent2D;

UENUM(BlueprintType)
enum class ECameraSystemMode : uint8
{
	Standby      UMETA(DisplayName = "Standby (Monitor Desligado)"),
	DockingMode  UMETA(DisplayName = "Modo Acoplamento (Câmera Ativa)")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOCKSIM_API UCFTVComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCFTVComponent();

	void InitializeCameras(USceneCaptureComponent2D* Front, USceneCaptureComponent2D* Rear, USceneCaptureComponent2D* Left, USceneCaptureComponent2D* Right, USceneCaptureComponent2D* Docking);
	void InitializeDockingCamera(USceneCaptureComponent2D* Docking);

	UFUNCTION(BlueprintCallable, Category = "Ship Systems|CCTV")
	void ToggleCameraMode();
	
	UFUNCTION(BlueprintPure, Category = "Ship Systems|CCTV")
	ECameraSystemMode GetCurrentMode() const { return CurrentCameraMode; }
	
	void InitializeDockingScreen(UMeshComponent* ScreenMesh);

private:
	ECameraSystemMode CurrentCameraMode = ECameraSystemMode::Standby;

	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Front;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Rear;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Left;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Right;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Docking;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DockingMaterial;

	void UpdateCameraPowerStates();
	void UpdateDockingCameraPowerStates();
};