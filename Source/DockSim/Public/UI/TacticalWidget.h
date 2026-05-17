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

	UPROPERTY(BlueprintReadOnly, Category = "Tactical | Flight")
	EThrustPowerMode CurrentThrustMode = EThrustPowerMode::Standard;

	UPROPERTY(BlueprintReadOnly, Category = "Tactical | Navigation")
	float AbsoluteVelocity = 0.0f;

private:
	UFUNCTION()
	void HandleThrustModeChanged(EThrustPowerMode NewMode);

	TWeakObjectPtr<class USpaceMovementComponent> CachedMovementComp;
	TWeakObjectPtr<class USpaceTelemetryComponent> CachedTelemetryComp;
};