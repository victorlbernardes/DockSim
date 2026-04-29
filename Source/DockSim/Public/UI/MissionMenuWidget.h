#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Core/SpacePlayerController.h" // Necessário para o enum EMissionState
#include "MissionMenuWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * Classe de UI que gerencia os estados de Pausa, Vitória e Derrota.
 * No Blueprint, a classe pai deve ser alterada para UMissionMenuWidget.
 */
UCLASS()
class DOCKSIM_API UMissionMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Configura o visual do menu com base no estado enviado pelo Controller */
	void SetupMenuState(EMissionState NewState);

protected:
	/** Chamado quando o Widget é inicializado */
	virtual void NativeConstruct() override;

	// --- COMPONENTES VINCULADOS (Devem ter o mesmo nome no Designer do WBP) ---
    
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Restart;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_MainMenu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Continue; // Opcional: visível apenas na Pausa

private:
	/** Funções de callback para os cliques */
	UFUNCTION()
	void OnRestartClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnContinueClicked();
};