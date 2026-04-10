#include "Components/SpaceResourceComponent.h"

USpaceResourceComponent::USpaceResourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // Esse componente não precisa de Tick! Ele é passivo.
}

void USpaceResourceComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentFuel = MaxFuel; // Enche o tanque quando a nave "nasce"
}

float USpaceResourceComponent::GetFuelPercent() const
{
	if (MaxFuel <= 0.0f) return 0.0f;
	return FMath::Clamp(CurrentFuel / MaxFuel, 0.0f, 1.0f);
}

bool USpaceResourceComponent::HasFuel() const
{
	return CurrentFuel > 0.0f;
}

bool USpaceResourceComponent::ConsumeFuel(float Amount)
{
	if (!HasFuel()) return false;

	// Subtrai o combustível e não deixa cair abaixo de zero
	CurrentFuel = FMath::Clamp(CurrentFuel - Amount, 0.0f, MaxFuel);
	if (CurrentFuel <= 0.0f)
	{
		OnFuelEmpty.Broadcast();
	}
	return true;
}