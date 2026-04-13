#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CFTVComponent.generated.h"

class USceneCaptureComponent2D;

// O Enum se mudou para cá!
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

	// Função para o Pawn entregar o hardware para este software gerenciar
	void InitializeCameras(USceneCaptureComponent2D* Front, USceneCaptureComponent2D* Rear, USceneCaptureComponent2D* Left, USceneCaptureComponent2D* Right, USceneCaptureComponent2D* Docking);
	void InitializeDockingCamera(USceneCaptureComponent2D* Docking);

	// A função pública que o Blueprint vai chamar para trocar a tela
	UFUNCTION(BlueprintCallable, Category = "Ship Systems|CCTV")
	void ToggleCameraMode();
	
	// Para o Widget do painel saber qual tela mostrar (Standby ou Imagem)
	UFUNCTION(BlueprintPure, Category = "Ship Systems|CCTV")
	ECameraSystemMode GetCurrentMode() const { return CurrentCameraMode; }

private:
	// Memória Interna do Estado
	ECameraSystemMode CurrentCameraMode = ECameraSystemMode::Standby;

	// Ponteiros Internos (O Hardware Emprestado)
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Front;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Rear;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Left;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Right;
	UPROPERTY() TObjectPtr<USceneCaptureComponent2D> Cam_Docking;

	// O cérebro que distribui a energia
	void UpdateCameraPowerStates();
	void UpdateDockingCameraPowerStates();
};