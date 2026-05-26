#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuManagerWidget.generated.h"

class UWidgetSwitcher;
class AShipCarouselManager;

UCLASS()
class DOCKSIM_API UMenuManagerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MENU_Switcher;
	
	UPROPERTY(BlueprintReadOnly, Category="Carousel")
	TObjectPtr<AShipCarouselManager> CachedCarouselManager;

public:
	UFUNCTION(BlueprintCallable, Category="Menu Navigation")
	void GoToShipSelection();

	UFUNCTION(BlueprintCallable, Category="Menu Navigation")
	void GoToControls();

	UFUNCTION(BlueprintCallable, Category="Menu Navigation")
	void GoToSettings();

	UFUNCTION(BlueprintCallable, Category="Menu Navigation")
	void GoToMainMenu();

	UFUNCTION(BlueprintCallable, Category="Menu Navigation")
	void QuitGame();

	UFUNCTION(BlueprintCallable, Category="Carousel Controls")
	void ShowNextShip();

	UFUNCTION(BlueprintCallable, Category="Carousel Controls")
	void ShowPrevShip();

private:
	void FindCarouselManager();
};