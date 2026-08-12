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

	CurrentState = EIntegrityState::Critical;
	OnHullCritical.Broadcast(); 
}