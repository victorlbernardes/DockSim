#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SpaceResourceComponent.h"
#include "EngineeringWidget.generated.h"

UCLASS()
class DOCKSIM_API UEngineeringWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION()
	void HandleFuelStatusChanged(EFuelStatus NewStatus);
	
protected:
	// Variáveis que o Blueprint vai ler para desenhar as barras
	UPROPERTY(BlueprintReadOnly, Category = "Ship Systems|Resources")
	float FuelPercent = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Ship Systems|Resources")
	EFuelStatus CurrentVisualStatus = EFuelStatus::Normal;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	TWeakObjectPtr<class USpaceResourceComponent> CachedResourceComp;
};