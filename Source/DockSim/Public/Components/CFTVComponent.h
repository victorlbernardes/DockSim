#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CFTVComponent.generated.h"

class USceneCaptureComponent2D;

// O Enum se mudou para cá!
UENUM(BlueprintType)
enum class ECameraSystemMode : uint8
{
	FlightMode  UMETA(DisplayName = "Modo de Voo (4 Câmeras)"),
	DockingMode UMETA(DisplayName = "Modo Acoplamento (1 Câmera)")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOCKSIM_API UCFTVComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCFTVComponent();

	// Função para o Pawn entregar o hardware para este software gerenciar
	void InitializeCameras(USceneCaptureComponent2D* Front, USceneCaptureComponent2D* Rear, USceneCaptureComponent2D* Left, USceneCaptureComponent2D* Right, USceneCaptureComponent2D* Docking);

	// A função pública que o Blueprint vai chamar para trocar a tela
	UFUNCTION(BlueprintCallable, Category = "Ship Systems|CCTV")
	void ToggleCameraMode();

private:
	// Memória Interna do Estado
	ECameraSystemMode CurrentCameraMode = ECameraSystemMode::FlightMode;

	// Ponteiros Internos (O Hardware Emprestado)
	UPROPERTY() USceneCaptureComponent2D* Cam_Front;
	UPROPERTY() USceneCaptureComponent2D* Cam_Rear;
	UPROPERTY() USceneCaptureComponent2D* Cam_Left;
	UPROPERTY() USceneCaptureComponent2D* Cam_Right;
	UPROPERTY() USceneCaptureComponent2D* Cam_Docking;

	// O cérebro que distribui a energia
	void UpdateCameraPowerStates();
};