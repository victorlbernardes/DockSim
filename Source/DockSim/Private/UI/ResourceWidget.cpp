#include "UI/ResourceWidget.h"
#include "Components/SpaceResourceComponent.h"
#include "GameFramework/Pawn.h"

void UResourceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		CachedResourceComp = PlayerPawn->FindComponentByClass<USpaceResourceComponent>();
        
		if (CachedResourceComp.IsValid())
		{
			CachedResourceComp->OnFuelStatusChanged.AddDynamic(this, &UResourceWidget::HandleFuelStatusChanged);
		}
	}
}

void UResourceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CachedResourceComp.IsValid())
	{
		FuelPercent = CachedResourceComp->GetFuelPercent();
	}
}

void UResourceWidget::HandleFuelStatusChanged(EFuelStatus NewStatus)
{
	CurrentVisualStatus = NewStatus;
}