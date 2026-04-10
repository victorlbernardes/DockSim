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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ShipMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CockpitCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpaceResourceComponent> ResourceComponent;
	
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
		
	void Move(const FInputActionValue& Value);
	void OnThrottle(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnToggleYaw(const FInputActionValue& Value);
	void OnToggleRoll(const FInputActionValue& Value);
	void OnPrecisionModeStart(const FInputActionValue& Value);
	void OnPrecisionModeEnd(const FInputActionValue& Value);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpaceMovementComponent> MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpaceTelemetryComponent> TelemetryComponent;
	
	ERotationMode CurrentRotState = ERotationMode::Locked;
	
};
