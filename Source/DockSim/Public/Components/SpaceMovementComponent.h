#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpaceMovementComponent.generated.h"

class UStaticMeshComponent;
class USpaceResourceComponent;

UENUM(BlueprintType)
enum class EThrustPowerMode : uint8
{
	Standard   UMETA(DisplayName = "Standard Thrust"),
	Precision  UMETA(DisplayName = "Precision RCS")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThrustModeChanged, EThrustPowerMode, NewMode);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USpaceMovementComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	USpaceMovementComponent();
	
	// NOVO: Função para alterar o modo pelo Cérebro
	void SetThrustPowerMode(EThrustPowerMode NewMode);

	// O Cérebro agora manda o "alvo" e não a força bruta
	void RequestTranslation(FVector TranslationInput);
	void AdjustThrottle(float ThrottleValue);
	void RequestRotation(FVector TorqueInput);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "Flight Dynamics")
	EThrustPowerMode GetCurrentPowerMode() const { return CurrentPowerMode; }
	
	UFUNCTION(BlueprintCallable, Category = "Flight Dynamics")
	float GetMasterPowerScale() const { return MasterPowerScale; }
	
	UPROPERTY(BlueprintAssignable, Category = "Ship|Events")
	FOnThrustModeChanged OnThrustModeChanged;
	
protected:	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics")
	float ThrustForce = 500000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics")
	float BrakeMultiplier;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics")
	float TorqueForce = 500000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics")
	float LinearDamping  = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics")
	float AngularDamping = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics|Calibration")
	float MouseSensitivity = 0.5f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics|Calibration")
	float InputCurveExponent = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics|Linear")
	float LinearInputSmoothing = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics|Rotation")
	float RotationInputSmoothing = 15.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics|Linear")
	float PrecisionModeMultiplier = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics|Resources")
	float FuelConsumptionRate = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Dynamics|Resources")
	float RotationFuelConsumptionRate = 1.0f;
	
private:
	TObjectPtr<UStaticMeshComponent> UpdatedMesh = nullptr;;
	UPROPERTY()
	TObjectPtr<USpaceResourceComponent> ResourceComp = nullptr;
	
	float MasterPowerScale = 0.0f;
	float CurrentSASIntensity = 0.0f;
	FVector CurrentSmoothedInput;
	FVector TargetInput;
	FVector TargetRotationInput;        
	FVector CurrentSmoothedRotation;
	EThrustPowerMode CurrentPowerMode = EThrustPowerMode::Standard;
};
