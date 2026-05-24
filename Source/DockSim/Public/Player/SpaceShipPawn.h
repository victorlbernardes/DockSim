#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "SpaceShipPawn.generated.h"

struct FInputActionValue;
class UStaticMeshComponent;
class UCameraComponent;
class UInputAction;
class USpaceMovementComponent;
class USpaceTelemetryComponent;
class USpaceResourceComponent;
class USpaceHullComponent;
class UCFTVComponent;
class USpaceAudioComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UWidgetComponent; 
class UShipHUDWidget;
class USpotLightComponent;

UENUM(BlueprintType)
enum class EShipFlightState : uint8
{
	FreeFlying      UMETA(DisplayName = "Voo Livre"),
	LandingMode     UMETA(DisplayName = "Modo de Pouso (Grampos Expostos)"),
	Docked          UMETA(DisplayName = "Atracado"),
	Disabled        UMETA(DisplayName = "Desativado / Destruído")
};

UENUM(BlueprintType)
enum class ESignalStatus : uint8
{
	Lost        UMETA(DisplayName = "Sinal Perdido (Fora de Alcance)"),
	Acquired    UMETA(DisplayName = "Sinal Adquirido (Na Zona)")
};

UENUM(BlueprintType)
enum class EDockingMonitorState : uint8
{
	Offline     UMETA(DisplayName = "Monitor Desligado (Preto)"),
	Searching   UMETA(DisplayName = "Buscando Sinal (Estática/Vermelho)"),
	Active      UMETA(DisplayName = "Conectado (Câmera CFTV)")
};

UENUM(BlueprintType)
enum class EPilotVisionMode : uint8
{
	Standard  UMETA(DisplayName = "Visão Padrão (Relaxado)"),
	FreeLook  UMETA(DisplayName = "Olhar ao Redor (Livre)")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDockingCompleted);
UCLASS()
class DOCKSIM_API ASpaceShipPawn : public APawn
{
	GENERATED_BODY()
		
public:
	ASpaceShipPawn();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	EShipFlightState GetCurrentFlightState() const { return CurrentFlightState; }
	
	UFUNCTION(BlueprintPure, Category = "Ship Systems|Docking")
	EDockingMonitorState GetMonitorState() const { return CurrentMonitorState; }

	UFUNCTION(BlueprintCallable, Category = "Ship Systems|Docking")
	void UpdateDockingSignal(ESignalStatus NewSignalStatus);
	void ExecuteHardcoreDocking(class ASpaceDockingPort* TargetPort);
	
	class USpaceHullComponent* GetHullComponent() const { return HullComponent; }
	
	USceneComponent* GetDockingPoint() const { return DockingPoint; }
		
	UFUNCTION()
	void HandleHullWarning(float ImpactSpeed);
	
	UFUNCTION()
	void HandleHullCritical();
	
	UPROPERTY(BlueprintAssignable, Category = "Ship Systems|Events")
	FOnDockingCompleted OnDockingCompleted;
	
	bool GetHeadlight() const { return bHeadlightsOn; }
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ShipMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CockpitCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpaceMovementComponent> MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpaceTelemetryComponent> TelemetryComponent;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpaceResourceComponent> ResourceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpaceHullComponent> HullComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpaceAudioComponent> AudioSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SensorShield;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|CCTV")
	TObjectPtr<UCFTVComponent> CFTVSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Docking")
	TObjectPtr<USceneComponent> DockingPoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Docking")
	TObjectPtr<USceneCaptureComponent2D> Cam_Docking; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Lights")
	TObjectPtr<USpotLightComponent> LeftHeadlight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Lights")
	TObjectPtr<USpotLightComponent> RightHeadlight;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UShipHUDWidget> HUDClass;
	
	UPROPERTY()
	TObjectPtr<UShipHUDWidget> MainHUDInstance;

	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> DynamicDockingRT;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RollAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleHeadlightsAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PrecisionAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FreeLookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DockAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Settings|Pilot Neck")
	float MaxLookUp = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Settings|Pilot Neck")
	float MaxLookDown = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Settings|Pilot Neck")
	float MaxLookLeftRight = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Settings|Pilot Neck")
	float LookSensitivity = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Settings|Damage")
	float MaxSafeImpactSpeed = 2.0f;
	
	UPROPERTY(BlueprintReadOnly, Category="Ship Systems|Lights")
	bool bHeadlightsOn = false;

	void Move(const FInputActionValue& Value);
	void OnThrottle(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void OnRoll(const FInputActionValue& Value);
	void OnPrecisionModeStart(const FInputActionValue& Value);
	void OnPrecisionModeEnd(const FInputActionValue& Value);
	void ToggleFreeLook(const FInputActionValue& Value);
	void OnDockButtonPressed(const FInputActionValue& Value);
	void OnPausePressed(const FInputActionValue& Value);
	void OnToggleHeadlights(const FInputActionValue& Value);
	
	UFUNCTION()
	void OnShipHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	EShipFlightState CurrentFlightState = EShipFlightState::FreeFlying;
	EPilotVisionMode CurrentVisionMode = EPilotVisionMode::Standard;
	ESignalStatus CurrentSignalStatus = ESignalStatus::Lost;
	EDockingMonitorState CurrentMonitorState = EDockingMonitorState::Offline;
	
	FVector PreImpactVelocity;

	FTimerHandle TelemetryTimerHandle;
	void UpdateDockingMonitor();
};