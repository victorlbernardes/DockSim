#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpaceHullComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHullCriticalSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHullWarningSignature, float, ImpactSpeed);

UENUM(BlueprintType)
enum class EIntegrityState : uint8
{
	Nominal     UMETA(DisplayName = "Nominal"),
	Warning     UMETA(DisplayName = "Avariado"),
	Critical    UMETA(DisplayName = "Destruído")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOCKSIM_API USpaceHullComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpaceHullComponent();

	void ProcessImpact(float ImpactSpeed);
	
	UPROPERTY(BlueprintAssignable, Category = "Hull|Events")
	FOnHullCriticalSignature OnHullCritical;

	UPROPERTY(BlueprintAssignable, Category = "Hull|Events")
	FOnHullWarningSignature OnHullWarning;

	UFUNCTION(BlueprintPure, Category = "Hull")
	EIntegrityState GetIntegrityState() const { return CurrentState; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Hull|Settings")
	float WarningThreshold = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Hull|Settings")
	float CriticalThreshold = 5.0f;

private:
	EIntegrityState CurrentState = EIntegrityState::Nominal;
};