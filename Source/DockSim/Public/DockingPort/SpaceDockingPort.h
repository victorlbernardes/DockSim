#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceDockingPort.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class UMaterialInstanceDynamic;
class ASpaceShipPawn;
class URotatingMovementComponent;
class UInterpToMovementComponent;

UENUM(BlueprintType)
enum class EDockingStatus : uint8
{
	None        UMETA(DisplayName = "Longe (Ciano Pulsante)"),
	Standby     UMETA(DisplayName = "Na Zona (Laranja Piscante)"),
	Granted     UMETA(DisplayName = "Acoplado (Verde Sólido)"),
	Denied      UMETA(DisplayName = "Negado (Vermelho Alerta)")
};

UCLASS()
class DOCKSIM_API ASpaceDockingPort : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpaceDockingPort();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<USceneComponent> DockingAnchor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UStaticMeshComponent> DockVisualRing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UBoxComponent> CaptureArea;
	
	UFUNCTION(BlueprintCallable, Category = "Docking Rules")
	EDockingStatus RequestDockingPermission(float ApproachSpeedMPS);
	
	UFUNCTION(BlueprintCallable, Category = "Docking")
	void SetDockingStatus(EDockingStatus NewStatus);
	
	UFUNCTION(BlueprintPure, Category = "Docking")
	FVector GetRealVelocity() const { return CalculatedVelocity; }

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void CalculateRealVelocity();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UStaticMeshComponent> DockStructureMesh;

	UFUNCTION()
	void OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking Rules")
	float MaxSafeDockingSpeed = 2.0f; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking Rules")
	EDockingStatus CurrentDockingStatus = EDockingStatus::None;
	
	UPROPERTY(VisibleAnywhere, Category = "Docking|Visuals")
	TObjectPtr<UStaticMeshComponent> SirenBulb;

	UPROPERTY(VisibleAnywhere, Category = "Docking|Visuals")
	TObjectPtr<UPointLightComponent> BeaconLight;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicSirenMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<class UBoxComponent> PrecisionDockingVolume;

	UFUNCTION()
	void OnPrecisionVolumeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void UpdateSirenLogic();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Visuals")
	TArray<TObjectPtr<UStaticMeshComponent>> RunwayLights;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking|Visuals")
	float RunwayRingRadius = 400.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking|Visuals")
	int32 NumberOfLights = 4;
	
	UPROPERTY()
	TObjectPtr<ASpaceShipPawn> ShipInZone;

	FTimerHandle DockingValidationTimer;

	UFUNCTION()
	void CheckContinuousDocking();

	UFUNCTION()
	void OnPrecisionVolumeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY()
	TObjectPtr<ASpaceShipPawn> ShipInApproachZone;

	UPROPERTY(EditDefaultsOnly, Category = "Docking|Visuals")
	float FastSpeedThreshold = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Docking|Visuals")
	float OptimalSpeedThreshold = 0.5f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Movement")
	TObjectPtr<URotatingMovementComponent> RotatingMovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking Rules|Difficulty")
	FRotator BaseRotationRate = FRotator(0.0f, 0.0f, 1.0f);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Movement")
	TObjectPtr<UInterpToMovementComponent> DriftMovementComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking Rules|Difficulty")
	FVector DriftOffset = FVector(0.0f, 800.0f, 0.0f); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking Rules|Difficulty")
	float DriftDuration = 20.0f;

private:
	FTimerHandle SirenTimerHandle;
	FTimerHandle VelocityTimerHandle;
	FVector LastLocation = FVector::ZeroVector;
	FVector CalculatedVelocity = FVector::ZeroVector;
};