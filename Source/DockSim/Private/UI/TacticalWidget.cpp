#include "UI/TacticalWidget.h"
#include "Components/SpaceTelemetryComponent.h"
#include "GameFramework/Pawn.h"

void UTacticalWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		// 1. Liga ao Componente de Movimento (Músculo)
		CachedMovementComp = PlayerPawn->FindComponentByClass<USpaceMovementComponent>();
		if (CachedMovementComp.IsValid())
		{
			// Assina o evento para mudar o modo de voo
			CachedMovementComp->OnThrustModeChanged.AddDynamic(this, &UTacticalWidget::HandleThrustModeChanged);
			CurrentThrustMode = CachedMovementComp->GetCurrentPowerMode();
		}

		// 2. Liga ao Componente de Telemetria (Olhos)
		CachedTelemetryComp = PlayerPawn->FindComponentByClass<USpaceTelemetryComponent>();
	}
}

void UTacticalWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Atualiza a velocidade absoluta todo frame
	if (CachedTelemetryComp.IsValid())
	{
		FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();
        
		// Cálculo da Magnitude (Velocidade Total) baseada nos seus dados de telemetria
		FVector VelVector(Data.VelForward, Data.VelRight, Data.VelUp);
		AbsoluteVelocity = VelVector.Size();
	}
}

void UTacticalWidget::HandleThrustModeChanged(EThrustPowerMode NewMode)
{
	CurrentThrustMode = NewMode;
}