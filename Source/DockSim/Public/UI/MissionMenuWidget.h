#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/SpacePlayerController.h" 
#include "MissionMenuWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class DOCKSIM_API UMissionMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetupMenuState(EMissionState NewState);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Restart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_MainMenu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Continue; 

private:
	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnContinueClicked();
};