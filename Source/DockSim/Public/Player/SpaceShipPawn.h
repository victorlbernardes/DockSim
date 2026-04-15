#pragma once

#include "CoreMinimal.h"
#include "DockingPort/SpaceDockingPort.h"
#include "GameFramework/Pawn.h"
#include "SpaceShipPawn.generated.h"


struct FInputActionValue;
class UStaticMeshComponent;
class UCameraComponent;
class UInputAction;
class USpaceMovementComponent;
class USpaceResourceComponent;
class UCFTVComponent;
class USceneCaptureComponent2D;
class UWidgetComponent;
class UWidgetSwitcher;

// O estado do sinal de rádio da estação
UENUM(BlueprintType)
enum class ESignalStatus : uint8
{
	Lost        UMETA(DisplayName = "Sinal Perdido (Fora de Alcance)"),
	Acquired    UMETA(DisplayName = "Sinal Adquirido (Na Zona)")
};

// A Máquina de Estados real do Monitor (O que o jogador vê)
UENUM(BlueprintType)
enum class EDockingMonitorState : uint8
{
	Offline     UMETA(DisplayName = "Monitor Desligado (Preto)"),
	Searching   UMETA(DisplayName = "Buscando Sinal (Estática/Vermelho)"),
	Active      UMETA(DisplayName = "Conectado (Câmera CFTV)")
};

UENUM(BlueprintType)
enum class EPilotVisionMode : uint8
{
	Standard  UMETA(DisplayName = "Visão Padrão (Relaxado)"),
	FreeLook  UMETA(DisplayName = "Olhar ao Redor (Livre)")
};

UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	Locked      UMETA(DisplayName = "Locked"),
	YawControl     UMETA(DisplayName = "Yaw Control"),
	RollControl    UMETA(DisplayName = "Roll Control"),
	YawAndRollControl  UMETA(DisplayName = "Yaw and Roll")
};

UCLASS()
class DOCKSIM_API ASpaceShipPawn : public APawn
{
	GENERATED_BODY()
		
public:
	ASpaceShipPawn();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// A função que a Estação chama ao invés do SetDockingSignal(bool)
	UFUNCTION(BlueprintCallable, Category = "Ship Systems|Docking")
	void UpdateDockingSignal(ESignalStatus NewSignalStatus);

	// Para o Widget saber exatamente o que desenhar
	UFUNCTION(BlueprintPure, Category = "Ship Systems|Docking")
	EDockingMonitorState GetMonitorState() const { return CurrentMonitorState; }
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UShipHUDWidget> HUDClass;
	UPROPERTY()
	TObjectPtr<UShipHUDWidget> MainHUDInstance;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ShipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CockpitCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> ToggleYawAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> ToggleRollAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> PrecisionAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> FreeLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> ToggleCameraAction;
		
	// ==========================================
	// SISTEMA DE CFTV (Câmeras do Cockpit)
	// ==========================================

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|CCTV")
	// TObjectPtr<USceneCaptureComponent2D> Cam_Front;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|CCTV")
	// TObjectPtr<USceneCaptureComponent2D> Cam_Left;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|CCTV")
	// TObjectPtr<USceneCaptureComponent2D> Cam_Right;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|CCTV")
	// TObjectPtr<USceneCaptureComponent2D> Cam_Rear;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|CCTV")
	TObjectPtr<USceneCaptureComponent2D> Cam_Docking; 
	
	// O Switcher que controlará a visibilidade do alerta no HUD
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> AlertSwitcher;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Systems|Pilot|Neck")
	float MaxLookUp = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Systems|Pilot|Neck")
	float MaxLookDown = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Systems|Pilot|Neck")
	float MaxLookLeftRight = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Systems|Pilot|Neck")
	float LookSensitivity = 2.0f;
		
	void Move(const FInputActionValue& Value);
	void OnThrottle(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnToggleYaw(const FInputActionValue& Value);
	void OnToggleRoll(const FInputActionValue& Value);
	void OnPrecisionModeStart(const FInputActionValue& Value);
	void OnPrecisionModeEnd(const FInputActionValue& Value);
	void OnFreeLookStart(const FInputActionValue& Value);
	void OnFreeLookEnd(const FInputActionValue& Value);
	void OnToggleCameraPressed();
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpaceMovementComponent> MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpaceTelemetryComponent> TelemetryComponent;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpaceResourceComponent> ResourceComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Software", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCFTVComponent> CFTVSystem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> LeftDisplayComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UWidgetComponent> RightDisplayComp;
	
	EPilotVisionMode CurrentVisionMode = EPilotVisionMode::Standard;
	ERotationMode CurrentRotState = ERotationMode::Locked;
	ESignalStatus CurrentSignalStatus = ESignalStatus::Lost;
	EDockingMonitorState CurrentMonitorState = EDockingMonitorState::Offline;
	void SetMonitorState(EDockingMonitorState NewState);
	
};
