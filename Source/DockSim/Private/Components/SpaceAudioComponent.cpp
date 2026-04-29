#include "Components/SpaceAudioComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Components/SpaceMovementComponent.h"
#include "Kismet/GameplayStatics.h"

USpaceAudioComponent::USpaceAudioComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USpaceAudioComponent::BeginPlay()
{
    Super::BeginPlay();
    TelemetryComp = GetOwner()->FindComponentByClass<USpaceTelemetryComponent>();
    MovementComp = GetOwner()->FindComponentByClass<USpaceMovementComponent>();

    if (Snd_AmbienceLoop)
    {
        AmbienceAudioComp = UGameplayStatics::SpawnSoundAttached(Snd_AmbienceLoop, GetOwner()->GetRootComponent());
    }

    if (Snd_EngineLoop)
    {
        EngineAudioComp = UGameplayStatics::SpawnSoundAttached(Snd_EngineLoop, GetOwner()->GetRootComponent());
        if (EngineAudioComp) 
        {
            // Impede que a Unreal apague o som se ele parar de tocar
            EngineAudioComp->bAutoDestroy = false; 
            EngineAudioComp->SetVolumeMultiplier(0.0f);
            
            // Garante que ele comece tocando
            if (!EngineAudioComp->IsPlaying())
            {
                EngineAudioComp->Play();
            }
        }
    }
}

void USpaceAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateEngineAudio(DeltaTime);
    UpdateProximityAlert(DeltaTime);
}

void USpaceAudioComponent::UpdateEngineAudio(float DeltaTime)
{
    if (!EngineAudioComp || !MovementComp) return;

    // 1. LEITURA CORRETA: Pegamos o movimento linear (WASD) e o Acelerador (Scroll)
    FVector TransInput = MovementComp->GetTargetTranslationInput();
    float Throttle = MovementComp->GetMasterPowerScale();
    
    // 2. CONDIÇÃO: Se houver qualquer esforço linear, o motor deve "Thrusting"
    if (Throttle > 0.01f || !TransInput.IsNearlyZero(0.01f))
    {
        CurrentAudioState = EEngineAudioState::Thrusting;
    }
    else
    {
        CurrentAudioState = EEngineAudioState::Silent;
    }

    // 3. CALCULO DE VOLUME
    float CombinedPower = FMath::Max(Throttle, TransInput.Size());
    float TargetVolume = (CurrentAudioState == EEngineAudioState::Thrusting) ? CombinedPower : 0.0f;
    
    // Garante um volume mínimo audível se o motor estiver ativo
    if (CurrentAudioState == EEngineAudioState::Thrusting && TargetVolume < 0.2f) TargetVolume = 0.2f;

    EngineAudioComp->SetVolumeMultiplier(FMath::FInterpTo(EngineAudioComp->VolumeMultiplier, TargetVolume, DeltaTime, 5.0f));
}

void USpaceAudioComponent::UpdateProximityAlert(float DeltaTime)
{
    if (!TelemetryComp || !Snd_ProximityBeep) return;

    // Acessa a struct de dados que já calculamos
    float Distance = TelemetryComp->GetLatestTelemetry().Distance;

    // Regra de silêncio do sensor
    if (Distance > 15.0f || Distance <= 0.1f) return;

    BeepTimer -= DeltaTime;
    if (BeepTimer <= 0.0f)
    {
        UGameplayStatics::PlaySoundAtLocation(this, Snd_ProximityBeep, GetOwner()->GetActorLocation(), 0.4f);
        // O intervalo diminui conforme a distância diminui (bip mais rápido)
        BeepTimer = FMath::Clamp(Distance / 15.0f, 0.08f, 1.0f);
    }
}