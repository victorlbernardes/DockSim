#include "Components/SpaceHullComponent.h"

USpaceHullComponent::USpaceHullComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpaceHullComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USpaceHullComponent::ProcessImpact(float ImpactSpeed)
{
	UE_LOG(LogTemp, Warning, TEXT("HullComponent registrou impacto a: %.1f m/s"), ImpactSpeed);

	if (CurrentState == EIntegrityState::Critical) return;

	if (ImpactSpeed < 0.2f) return;

	if (ImpactSpeed >= CriticalThreshold)
	{
		CurrentState = EIntegrityState::Critical;
		OnHullCritical.Broadcast(); 
	}
	else if (ImpactSpeed >= WarningThreshold)
	{
		CurrentState = EIntegrityState::Warning;
		OnHullWarning.Broadcast(ImpactSpeed); 
	}
}