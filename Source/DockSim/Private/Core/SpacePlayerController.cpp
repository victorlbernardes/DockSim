#include "Core/SpacePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UI/ShipHUDWidget.h"
#include "UI/MissionMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Core/DockSimGameMode.h"

class ADockSimGameMode;

ASpacePlayerController::ASpacePlayerController()
{
}

void ASpacePlayerController::BeginPlay()
{
    Super::BeginPlay();

    bool bShouldHideForCinematic = false;
    if (ADockSimGameMode* GM = Cast<ADockSimGameMode>(GetWorld()->GetAuthGameMode()))
    {
        bShouldHideForCinematic = GM->bStartInCinematic;
    }

    if (HUDClass)
    {
        MainHUDInstance = CreateWidget<UShipHUDWidget>(this, HUDClass);
        if (MainHUDInstance)
        {
            if (!bShouldHideForCinematic)
            {
                MainHUDInstance->AddToViewport();
            }
            else
            {
                CurrentMissionState = EMissionState::Cinematic;
            }
        }
    }
    
    if (HUDClass)
    {
        MainHUDInstance = CreateWidget<UShipHUDWidget>(this, HUDClass);
        if (MainHUDInstance)
        {
            if (CurrentMissionState != EMissionState::Cinematic)
            {
                MainHUDInstance->AddToViewport();
            }
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
    if (NewState == EMissionState::Cinematic)
    {
        bShowMouseCursor = false;
        if (MainHUDInstance) 
        {
            MainHUDInstance->RemoveFromParent();
            MainHUDInstance->SetVisibility(ESlateVisibility::Hidden);
        }
        if (GetPawn()) GetPawn()->DisableInput(this);
    }
    else if (NewState == EMissionState::Playing)
    {
        bShowMouseCursor = false;
        SetInputMode(FInputModeGameOnly());
        SetPause(false);
        
        if (GetPawn()) GetPawn()->EnableInput(this);
        if (MainHUDInstance)
        {
            if (!MainHUDInstance->IsInViewport())
            {
                MainHUDInstance->AddToViewport();
            }
            MainHUDInstance->SetVisibility(ESlateVisibility::Visible);
        }

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