#pragma once

#include "CoreMinimal.h"
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
	
};
