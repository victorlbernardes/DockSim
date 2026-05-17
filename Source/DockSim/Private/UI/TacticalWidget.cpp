#include "UI/TacticalWidget.h"
#include "Components/SpaceTelemetryComponent.h"
#include "GameFramework/Pawn.h"

void UTacticalWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		CachedMovementComp = PlayerPawn->FindComponentByClass<USpaceMovementComponent>();
		if (CachedMovementComp.IsValid())
		{
			CachedMovementComp->OnThrustModeChanged.AddDynamic(this, &UTacticalWidget::HandleThrustModeChanged);
			CurrentThrustMode = CachedMovementComp->GetCurrentPowerMode();
		}

		CachedTelemetryComp = PlayerPawn->FindComponentByClass<USpaceTelemetryComponent>();
	}
}

void UTacticalWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CachedTelemetryComp.IsValid())
	{
		FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();
        
		FVector VelVector(Data.VelForward, Data.VelRight, Data.VelUp);
		AbsoluteVelocity = VelVector.Size();
	}
}

void UTacticalWidget::HandleThrustModeChanged(EThrustPowerMode NewMode)
{
	CurrentThrustMode = NewMode;
}