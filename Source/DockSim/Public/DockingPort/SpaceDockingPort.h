#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpaceDockingPort.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UPointLightComponent;

UCLASS()
class DOCKSIM_API ASpaceDockingPort : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpaceDockingPort();

	// O Ponto Matemático Exato de Acoplamento
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<USceneComponent> DockingAnchor;

	// O Volume invisível que detecta se a nave está perto o suficiente para ligar os sensores
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UBoxComponent> CaptureArea;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UPointLightComponent> BeaconLight;

protected:
	virtual void BeginPlay() override;

	// Uma raiz vazia ajuda na modularidade (para anexar em outras estações depois)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<USceneComponent> DefaultRoot;

	// A Malha Física (O "Anel" ou "Garagem")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Docking|Components")
	TObjectPtr<UStaticMeshComponent> DockStructureMesh;

	// Funções que o Trigger Box vai chamar quando a nave entrar/sair
	UFUNCTION()
	void OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	FTimerHandle BlinkTimerHandle;

	void ToggleBeaconLight();
	
};