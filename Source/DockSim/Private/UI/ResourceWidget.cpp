#include "UI/ResourceWidget.h"
#include "Components/SpaceResourceComponent.h"
#include "GameFramework/Pawn.h"

// 1. NATIVECONSTRUCT: Roda apenas UMA VEZ quando a tela liga.
void UResourceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Procuramos o componente apenas uma vez ao ligar a tela
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		CachedResourceComp = PlayerPawn->FindComponentByClass<USpaceResourceComponent>();
        
		if (CachedResourceComp.IsValid())
		{
			// ASSINATURA SEGURA: Ocorre apenas uma vez!
			CachedResourceComp->OnFuelStatusChanged.AddDynamic(this, &UResourceWidget::HandleFuelStatusChanged);
		}
	}
}

// 2. NATIVETICK: Roda todo frame. Deve ser o mais leve possível.
void UResourceWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// O Tick tem uma única responsabilidade: fazer a barrinha descer suavemente.
	if (CachedResourceComp.IsValid())
	{
		FuelPercent = CachedResourceComp->GetFuelPercent();
	}
}

// 3. O RECEPTOR DO EVENTO
void UResourceWidget::HandleFuelStatusChanged(EFuelStatus NewStatus)
{
	// Apenas repassa a cor para o Blueprint
	CurrentVisualStatus = NewStatus;
}