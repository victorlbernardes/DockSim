#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ShipHUDWidget.generated.h"

class USpaceTelemetryComponent;
struct FSpaceTelemetryData; 

UENUM(BlueprintType)
enum class EShipMovementState : uint8
{
	Moving      UMETA(DisplayName = "Em Movimento (Derivando)"),
	Stabilized  UMETA(DisplayName = "Inércia Zero (Travado)")
};

UCLASS()
class DOCKSIM_API UShipHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Flight UI")
	void OnTelemetryUpdated(const FSpaceTelemetryData& TelemetryData);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Velocity;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* IMG_PitchLadder;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* IMG_Prograde;
	
	UPROPERTY(EditDefaultsOnly, Category = "Flight UI|Icons")
	TObjectPtr<class UTexture2D> Tex_Prograde;

	UPROPERTY(EditDefaultsOnly, Category = "Flight UI|Icons")
	TObjectPtr<class UTexture2D> Tex_Retrograde;

private:
	
	UFUNCTION()
	void UpdateContinuousTelemetry();

	UPROPERTY()
	TObjectPtr<USpaceTelemetryComponent> CachedTelemetryComp;
	
	UPROPERTY()
	UMaterialInstanceDynamic* PitchLadderMID;

	FTimerHandle TelemetryTimerHandle;
	EShipMovementState CurrentMovementState = EShipMovementState::Stabilized;
};