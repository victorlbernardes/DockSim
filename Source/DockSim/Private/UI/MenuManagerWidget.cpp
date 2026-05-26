#include "UI/MenuManagerWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/ShipCarouselManager.h"

void UMenuManagerWidget::NativeConstruct()
{
	FindCarouselManager();
	Super::NativeConstruct();

	if (MENU_Switcher) MENU_Switcher->SetActiveWidgetIndex(0);
	if (CachedCarouselManager) CachedCarouselManager->HideCarousel();
}

void UMenuManagerWidget::FindCarouselManager()
{
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AShipCarouselManager::StaticClass());
	if (FoundActor)
	{
		CachedCarouselManager = Cast<AShipCarouselManager>(FoundActor);
	}
}

void UMenuManagerWidget::GoToShipSelection()
{
	if (MENU_Switcher) MENU_Switcher->SetActiveWidgetIndex(1);
	if (CachedCarouselManager) CachedCarouselManager->ShowCarousel();
}

void UMenuManagerWidget::GoToControls()
{
	if (MENU_Switcher) MENU_Switcher->SetActiveWidgetIndex(2);
}

void UMenuManagerWidget::GoToSettings()
{
	if (MENU_Switcher) MENU_Switcher->SetActiveWidgetIndex(3);
}

void UMenuManagerWidget::GoToMainMenu()
{
	if (MENU_Switcher && MENU_Switcher->GetActiveWidgetIndex() == 1)
	{
		if (CachedCarouselManager) CachedCarouselManager->HideCarousel();
	}
	if (MENU_Switcher) MENU_Switcher->SetActiveWidgetIndex(0);
}

void UMenuManagerWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UMenuManagerWidget::ShowNextShip()
{
	if (CachedCarouselManager) CachedCarouselManager->NextShip();
}

void UMenuManagerWidget::ShowPrevShip()
{
	if (CachedCarouselManager) CachedCarouselManager->PreviousShip();
}