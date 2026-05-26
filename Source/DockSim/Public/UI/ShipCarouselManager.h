#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipCarouselManager.generated.h"

class UShipDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnShipDataChanged,
	UShipDataAsset*,
	NewShipData
);

UCLASS()
class DOCKSIM_API AShipCarouselManager : public AActor
{
	GENERATED_BODY()

public:
	AShipCarouselManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Carousel")
	TArray<TObjectPtr<UShipDataAsset>> ShipDataCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Carousel")
	int32 CurrentIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Carousel")
	TObjectPtr<USceneComponent> ShipSpawnPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Carousel")
	TObjectPtr<class UStaticMeshComponent> PreviewMeshComponent;

public:
	UPROPERTY(BlueprintAssignable, Category="Carousel")
	FOnShipDataChanged OnShipDataChanged;

	UFUNCTION(BlueprintCallable, Category="Carousel")
	void ShowCarousel();

	UFUNCTION(BlueprintCallable, Category="Carousel")
	void HideCarousel();

	UFUNCTION(BlueprintCallable, Category="Carousel")
	void NextShip();

	UFUNCTION(BlueprintCallable, Category="Carousel")
	void PreviousShip();

protected:
	void UpdateShipDisplay(int32 Index);
};