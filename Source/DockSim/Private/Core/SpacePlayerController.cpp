#include "Core/SpacePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/ShipHUDWidget.h"
#include "UI/MissionMenuWidget.h"
#include "Blueprint/UserWidget.h"

ASpacePlayerController::ASpacePlayerController()
{
    // Construtor padrão
}

void ASpacePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Cria o HUD principal (Telemetria/Sinal) no início do jogo
    if (HUDClass)
    {
        MainHUDInstance = CreateWidget<UShipHUDWidget>(this, HUDClass);
        if (MainHUDInstance)
        {
            MainHUDInstance->AddToViewport();
        }
    }
    
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (ShipMappingContext)
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(ShipMappingContext, 0);
        }
    }
    SetMissionState(EMissionState::Playing);
}


void ASpacePlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
}

void ASpacePlayerController::UpdateSignalAlert(ESignalStatus NewStatus)
{
    if (MainHUDInstance)
    {
        MainHUDInstance->UpdateSignalAlert(NewStatus);
    }
}

void ASpacePlayerController::SetMissionState(EMissionState NewState)
{
    if (CurrentMissionState == NewState) return;
    
    CurrentMissionState = NewState;
    HandleStateChange(NewState);
}

void ASpacePlayerController::HandleStateChange(EMissionState NewState)
{
    if (NewState == EMissionState::Playing)
    {
        // --- VOLTANDO AO VOO ---
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());
        SetPause(false);

        if (ActiveMenuInstance)
        {
            ActiveMenuInstance->RemoveFromParent();
            ActiveMenuInstance = nullptr;
        }
    }
    else
    {
        // --- PAUSA, VITÓRIA OU DERROTA ---
        bShowMouseCursor = true;
        
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetWidgetToFocus(nullptr); 
        SetInputMode(InputMode);
        
        // Pausa o tempo do mundo (física, timers, etc)
        SetPause(true);

        // Garante que o widget de menu existe e está configurado
        if (!ActiveMenuInstance && MissionMenuClass)
        {
            ActiveMenuInstance = CreateWidget<UMissionMenuWidget>(this, MissionMenuClass);
        }
       
        if (ActiveMenuInstance)
        {
            if (!ActiveMenuInstance->IsInViewport())
            {
                ActiveMenuInstance->AddToViewport();
            }
            
            // O Menu se auto-ajusta (Texto/Cor) com base no Enum
            ActiveMenuInstance->SetupMenuState(NewState); 
        }
    }
}

void ASpacePlayerController::TogglePauseMenu()
{
    // Não permite pausar se a missão já acabou
    if (CurrentMissionState == EMissionState::Victory || CurrentMissionState == EMissionState::GameOver) return;

    if (CurrentMissionState == EMissionState::Playing)
    {
        SetMissionState(EMissionState::Paused);
    }
    else
    {
        SetMissionState(EMissionState::Playing);
    }
}