#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpaceResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFuelEmpty);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOCKSIM_API USpaceResourceComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	USpaceResourceComponent();
	// Funções Públicas (A API do seu Tanque)
	UFUNCTION(BlueprintCallable, Category = "Ship Resources")
	float GetFuelPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Ship Resources")
	bool HasFuel() const;

	// Retorna verdadeiro se conseguiu consumir, falso se o tanque secou
	bool ConsumeFuel(float Amount);
	
	UPROPERTY(BlueprintAssignable, Category = "Ship Resources|Events")
	FOnFuelEmpty OnFuelEmpty;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Resources|Fuel")
	float MaxFuel = 100.0f;

	// Não queremos que ninguem edite isso direto no Blueprint, apenas leia
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Resources|Fuel")
	float CurrentFuel = 0.0f;
};
