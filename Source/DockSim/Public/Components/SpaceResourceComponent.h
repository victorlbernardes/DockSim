#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpaceResourceComponent.generated.h"

// 1. DECLARAÇÃO DO ENUM DE STATUS DO COMBUSTÍVEL
UENUM(BlueprintType)
enum class EFuelStatus : uint8
{
	Normal      UMETA(DisplayName = "Normal"),
	LowFuel     UMETA(DisplayName = "Reserva Baixa (Aviso)"),
	Empty       UMETA(DisplayName = "Tanque Vazio")
};

// 2. O NOVO DELEGATE (Agora ele envia o Enum junto com o aviso!)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFuelStatusChanged, EFuelStatus, NewStatus);

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
	FOnFuelStatusChanged OnFuelStatusChanged;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Resources|Fuel")
	float MaxFuel = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Resources|Fuel")
	float LowFuelThreshold = 0.15f; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Resources|Fuel")
	float CurrentFuel = 0.0f;

private:
	EFuelStatus CurrentFuelStatus = EFuelStatus::Normal;
};
