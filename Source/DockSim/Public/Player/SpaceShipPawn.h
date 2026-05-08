#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SphereComponent.h"
#include "SpaceShipPawn.generated.h"

// -------------------------------------------------------------
// FORWARD DECLARATIONS (Melhora o tempo de compilação)
// -------------------------------------------------------------
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

// -------------------------------------------------------------
// ENUMS DE ESTADO
// -------------------------------------------------------------
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

// -------------------------------------------------------------
// CLASSE PRINCIPAL
// -------------------------------------------------------------
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDockingCompleted);
UCLASS()
class DOCKSIM_API ASpaceShipPawn : public APawn
{
	GENERATED_BODY()
		
public:
	ASpaceShipPawn();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Acesso aos Estados
	EShipFlightState GetCurrentFlightState() const { return CurrentFlightState; }
	
	UFUNCTION(BlueprintPure, Category = "Ship Systems|Docking")
	EDockingMonitorState GetMonitorState() const { return CurrentMonitorState; }

	// Interação com a Estação Espacial
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
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ==========================================
	// COMPONENTES DA NAVE
	// ==========================================
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

	// ==========================================
	// SISTEMA DE DOCKING, CFTV E MONITORES 3D
	// ==========================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|CCTV")
	TObjectPtr<UCFTVComponent> CFTVSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Docking")
	TObjectPtr<USceneComponent> DockingPoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Docking")
	TObjectPtr<USceneCaptureComponent2D> Cam_Docking; 

	// A carcaça de plástico do monitor no cockpit
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Docking")
	TObjectPtr<UStaticMeshComponent> DockingScreenMesh;

	// A tela digital brilhante que exibe a interface
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Docking")
	TObjectPtr<UWidgetComponent> DockingMonitorComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Lights")
	TObjectPtr<USpotLightComponent> LeftHeadlight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Systems|Lights")
	TObjectPtr<USpotLightComponent> RightHeadlight;
	// ==========================================
	// INTERFACE (UI) & RENDERS DINÂMICOS
	// ==========================================
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UShipHUDWidget> HUDClass;
	
	UPROPERTY()
	TObjectPtr<UShipHUDWidget> MainHUDInstance;

	// O arquivo de vídeo gerado dinamicamente para a câmera de ré
	UPROPERTY(Transient)
	TObjectPtr<UTextureRenderTarget2D> DynamicDockingRT;

	// ==========================================
	// INPUTS (Enhanced Input)
	// ==========================================
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

	// ==========================================
	// CONFIGURAÇÕES DA NAVE (Ajustáveis no Editor)
	// ==========================================
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

	// ==========================================
	// FUNÇÕES DE INPUT E LÓGICA
	// ==========================================
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
	
	// Callbacks de Física e Dano
	UFUNCTION()
	void OnShipHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnSensorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	// Máquinas de Estado e Variáveis Privadas
	EShipFlightState CurrentFlightState = EShipFlightState::FreeFlying;
	EPilotVisionMode CurrentVisionMode = EPilotVisionMode::Standard;
	ESignalStatus CurrentSignalStatus = ESignalStatus::Lost;
	EDockingMonitorState CurrentMonitorState = EDockingMonitorState::Offline;
	
	FVector PreImpactVelocity;

	FTimerHandle TelemetryTimerHandle;
	void UpdateDockingMonitor();
};