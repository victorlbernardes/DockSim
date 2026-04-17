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
	// Log para você ter um histórico no Output Log
	UE_LOG(LogTemp, Warning, TEXT("HullComponent registrou impacto a: %.1f m/s"), ImpactSpeed);

	if (CurrentState == EIntegrityState::Critical) return;

	// Ignora poeira espacial ou toques muito suaves (menos de 0.2 m/s)
	// Esse valor de 0.2 agora funciona perfeito para QUALQUER nave!
	if (ImpactSpeed < 0.2f) return;

	// Avalia a integridade com base na velocidade pura
	if (ImpactSpeed >= CriticalThreshold) // Ex: 5.0f no Blueprint
	{
		CurrentState = EIntegrityState::Critical;
		OnHullCritical.Broadcast(); 
	}
	else if (ImpactSpeed >= WarningThreshold) // Ex: 2.0f no Blueprint
	{
		CurrentState = EIntegrityState::Warning;
		OnHullWarning.Broadcast(ImpactSpeed); // Passa a velocidade para a UI se quiser exibir
	}
}