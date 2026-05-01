#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceDockingPort.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPointLightComponent;
class UMaterialInstanceDynamic;

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

	// 1. Volta a ser USceneComponent (Invisível, Matemática Pura)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<USceneComponent> DockingAnchor;

	// 2. Cria um componente só para a arte
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UStaticMeshComponent> DockVisualRing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UBoxComponent> CaptureArea;
	
	UFUNCTION(BlueprintCallable, Category = "Docking Rules")
	EDockingStatus RequestDockingPermission(float ApproachSpeedMPS);
	
	UFUNCTION(BlueprintCallable, Category = "Docking")
	void SetDockingStatus(EDockingStatus NewStatus);

protected:
	virtual void BeginPlay() override;

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
	
	// A Nova Zona Minúscula (O Berço)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<class UBoxComponent> PrecisionDockingVolume;

	// Função de checagem do toque
	UFUNCTION()
	void OnPrecisionVolumeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void UpdateSirenLogic();
	
	// O Array que vai guardar os ponteiros das nossas luzes procedurais
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Visuals")
	TArray<TObjectPtr<UStaticMeshComponent>> RunwayLights;

	// Opcional: Variáveis para você brincar com o tamanho do anel depois
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking|Visuals")
	float RunwayRingRadius = 400.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Docking|Visuals")
	int32 NumberOfLights = 4;

private:
	FTimerHandle SirenTimerHandle;
};