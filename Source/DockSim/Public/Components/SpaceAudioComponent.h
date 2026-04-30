#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpaceAudioComponent.generated.h"

// Estados do Motor para evitar Booleans
UENUM(BlueprintType)
enum class EEngineAudioState : uint8
{
	Silent      UMETA(DisplayName = "Mudo"),
	Thrusting   UMETA(DisplayName = "Propulsão Ativa")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOCKSIM_API USpaceAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	USpaceAudioComponent();
	void PlayDockingSuccessSound();
	void PlayDockingFailSound();
	void PlayShipCrashSound();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = "Space Audio|Assets")
	TObjectPtr<USoundBase> Snd_AmbienceLoop;

	UPROPERTY(EditDefaultsOnly, Category = "Space Audio|Assets")
	TObjectPtr<USoundBase> Snd_EngineLoop;

	UPROPERTY(EditDefaultsOnly, Category = "Space Audio|Assets")
	TObjectPtr<USoundBase> Snd_ProximityBeep;
	
	UPROPERTY(EditDefaultsOnly, Category = "Space Audio|Assets")
	TObjectPtr<USoundBase> Snd_DockingSuccess;
	
	UPROPERTY(EditDefaultsOnly, Category = "Space Audio|Assets")
	TObjectPtr<USoundBase> Snd_DockingFail;
	
	UPROPERTY(EditDefaultsOnly, Category = "Space Audio|Assets")
	TObjectPtr<USoundBase> Snd_ShipCrash;

private:
	UPROPERTY() TObjectPtr<UAudioComponent> AmbienceAudioComp;
	UPROPERTY() TObjectPtr<UAudioComponent> EngineAudioComp;
	UPROPERTY() class USpaceTelemetryComponent* TelemetryComp;
	UPROPERTY() class USpaceMovementComponent* MovementComp;

	EEngineAudioState CurrentAudioState = EEngineAudioState::Silent;

	void UpdateEngineAudio(float DeltaTime);
	void UpdateProximityAlert(float DeltaTime);
    
	float BeepTimer = 0.0f;
};