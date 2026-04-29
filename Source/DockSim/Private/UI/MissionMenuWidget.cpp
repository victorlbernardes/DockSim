#include "UI/MissionMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Core/SpacePlayerController.h"

void UMissionMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Vincula os eventos de clique às funções C++
    if (BTN_Restart)
        BTN_Restart->OnClicked.AddDynamic(this, &UMissionMenuWidget::OnRestartClicked);

    if (BTN_MainMenu)
        BTN_MainMenu->OnClicked.AddDynamic(this, &UMissionMenuWidget::OnMainMenuClicked);

    if (BTN_Continue)
        BTN_Continue->OnClicked.AddDynamic(this, &UMissionMenuWidget::OnContinueClicked);
}

void UMissionMenuWidget::SetupMenuState(EMissionState NewState)
{
    if (!TXT_Title) return;

    // Reset padrão: Esconder botão de continuar (só aparece na pausa)
    if (BTN_Continue) BTN_Continue->SetVisibility(ESlateVisibility::Collapsed);

    switch (NewState)
    {
    case EMissionState::Paused:
        TXT_Title->SetText(FText::FromString(TEXT("SISTEMAS EM ESPERA")));
        TXT_Title->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
        if (BTN_Continue) BTN_Continue->SetVisibility(ESlateVisibility::Visible);
        break;

    case EMissionState::Victory:
        TXT_Title->SetText(FText::FromString(TEXT("CARGA ENTREGUE - SUCESSO")));
        TXT_Title->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
        break;

    case EMissionState::GameOver:
        TXT_Title->SetText(FText::FromString(TEXT("SINAL PERDIDO - CASCO DESTRUÍDO")));
        TXT_Title->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
        break;
        
    default:
        break;
    }
}

void UMissionMenuWidget::OnContinueClicked()
{
    if (ASpacePlayerController* PC = Cast<ASpacePlayerController>(GetOwningPlayer()))
    {
        PC->SetMissionState(EMissionState::Playing);
    }
}

void UMissionMenuWidget::OnRestartClicked()
{
    // Obtém o nome do mapa atual e recarrega
    FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
    UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
}

void UMissionMenuWidget::OnMainMenuClicked()
{
    // Substitua "MainMenuMap" pelo nome real do seu mapa de menu
    UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenuMap"));
}