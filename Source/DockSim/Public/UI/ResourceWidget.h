#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SpaceResourceComponent.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Components/TextBlock.h"
#include "ResourceWidget.generated.h"

UCLASS()
class DOCKSIM_API UResourceWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UFUNCTION()
	void HandleFuelStatusChanged(EFuelStatus NewStatus);
	
protected:
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_FuelPercent;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TXT_ThrustMode;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_Fuel;	

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeConstruct() override;

private:
	TWeakObjectPtr<class USpaceResourceComponent> CachedResourceComp;
	TWeakObjectPtr<class USpaceTelemetryComponent> CachedTelemetryComp;
};