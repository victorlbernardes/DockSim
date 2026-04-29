#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpaceMovementComponent.h"
#include "SpaceTelemetryComponent.generated.h"

class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FSpaceTelemetryData
{
	GENERATED_BODY()

	// Engine Scale
	UPROPERTY(BlueprintReadOnly) float CurrentEngineScale = 0.0f;
	
	// Velocidades Locais (m/s)
	UPROPERTY(BlueprintReadOnly) float VelForward = 0.0f;
	UPROPERTY(BlueprintReadOnly) float VelRight   = 0.0f;
	UPROPERTY(BlueprintReadOnly) float VelUp      = 0.0f;
	UPROPERTY(BlueprintReadOnly) float VelTotal = 0.0f;

	// Atitude (Graus)
	UPROPERTY(BlueprintReadOnly) float Pitch = 0.0f;
	UPROPERTY(BlueprintReadOnly) float Yaw   = 0.0f;
	UPROPERTY(BlueprintReadOnly) float Roll  = 0.0f;
    
	UPROPERTY(BlueprintReadOnly) EThrustPowerMode ThrustMode = EThrustPowerMode::Standard;

	// --- NOVOS DADOS PARA O MONITOR DE DOCKING ---
	UPROPERTY(BlueprintReadOnly) float Distance = 0.0f;
	UPROPERTY(BlueprintReadOnly) float TotalAlignment = 0.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOCKSIM_API USpaceTelemetryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpaceTelemetryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// As variáveis que o HUD vai ler (BlueprintReadOnly)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	float ForwardSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	float LateralSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	float VerticalSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Telemetry")
	float TotalSpeed;
	
	FSpaceTelemetryData GetLatestTelemetry();
	bool IsReadyForHardLock() const;
	float ApplyPsychologicalZero(float Value) ;
protected:
	virtual void BeginPlay() override;

	// O Multiplicador do seu GDD para criar a "Ilusão de Velocidade"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Telemetry Settings")
	float SimulationMultiplier = 100.0f;

private:
	TObjectPtr<UStaticMeshComponent> TargetMesh;
};