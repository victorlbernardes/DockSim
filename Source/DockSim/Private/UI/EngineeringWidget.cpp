#include "UI/EngineeringWidget.h"
#include "Components/SpaceResourceComponent.h"

void UEngineeringWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!CachedResourceComp.IsValid())
	{
		APawn* PlayerPawn = GetOwningPlayerPawn();
		if (PlayerPawn)
		{
			CachedResourceComp = PlayerPawn->FindComponentByClass<USpaceResourceComponent>();
		}
	}

	if (CachedResourceComp.IsValid())
	{
		CachedResourceComp->OnFuelStatusChanged.AddDynamic(this, &UEngineeringWidget::HandleFuelStatusChanged);
		FuelPercent = CachedResourceComp->GetFuelPercent();
	}
	
}

void UEngineeringWidget::HandleFuelStatusChanged(EFuelStatus NewStatus)
{
	CurrentVisualStatus = NewStatus;
}