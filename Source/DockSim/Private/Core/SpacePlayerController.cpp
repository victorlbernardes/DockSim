#include "Core/SpacePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/ShipHUDWidget.h"
#include "UI/MissionMenuWidget.h"
#include "Blueprint/UserWidget.h"

ASpacePlayerController::ASpacePlayerController()
{
}

void ASpacePlayerController::BeginPlay()
{
    Super::BeginPlay();

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
        bShowMouseCursor = true;
        
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetWidgetToFocus(nullptr); 
        SetInputMode(InputMode);
        
        SetPause(true);

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
            
            ActiveMenuInstance->SetupMenuState(NewState); 
        }
    }
}

void ASpacePlayerController::TogglePauseMenu()
{
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