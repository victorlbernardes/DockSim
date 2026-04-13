#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SpaceMovementComponent.h" 
#include "TacticalWidget.generated.h"

UCLASS()
class DOCKSIM_API UTacticalWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Dados para o Blueprint ler
	UPROPERTY(BlueprintReadOnly, Category = "Tactical | Flight")
	EThrustPowerMode CurrentThrustMode = EThrustPowerMode::Standard;

	UPROPERTY(BlueprintReadOnly, Category = "Tactical | Navigation")
	float AbsoluteVelocity = 0.0f;

private:
	// Função para reagir à mudança de modo de voo
	UFUNCTION()
	void HandleThrustModeChanged(EThrustPowerMode NewMode);

	// Caches dos componentes
	TWeakObjectPtr<class USpaceMovementComponent> CachedMovementComp;
	TWeakObjectPtr<class USpaceTelemetryComponent> CachedTelemetryComp;
};