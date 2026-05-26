#include "UI/ShipCarouselManager.h"
#include "Data/ShipDataAsset.h" 
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

AShipCarouselManager::AShipCarouselManager()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	ShipSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ShipSpawnPoint"));
	ShipSpawnPoint->SetupAttachment(Root);

	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComponent"));
	PreviewMeshComponent->SetupAttachment(ShipSpawnPoint);
	PreviewMeshComponent->SetCollisionProfileName(TEXT("NoCollision")); 
}

void AShipCarouselManager::BeginPlay()
{
	Super::BeginPlay();
	HideCarousel();
}

void AShipCarouselManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PreviewMeshComponent)
	{
		FRotator Rotation = PreviewMeshComponent->GetRelativeRotation();
		Rotation.Yaw += DeltaTime * 10.f;
		PreviewMeshComponent->SetRelativeRotation(Rotation);
	}
}

void AShipCarouselManager::ShowCarousel()
{
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true); 

	UpdateShipDisplay(CurrentIndex);
}

void AShipCarouselManager::HideCarousel()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false); 
}

void AShipCarouselManager::NextShip()
{
	if (ShipDataCollection.Num() <= 0) return;

	CurrentIndex = (CurrentIndex + 1) % ShipDataCollection.Num();
	UpdateShipDisplay(CurrentIndex);
}

void AShipCarouselManager::PreviousShip()
{
	if (ShipDataCollection.Num() <= 0) return;

	CurrentIndex = (CurrentIndex - 1 + ShipDataCollection.Num()) % ShipDataCollection.Num();
	UpdateShipDisplay(CurrentIndex);
}

void AShipCarouselManager::UpdateShipDisplay(int32 Index)
{
	if (!ShipDataCollection.IsValidIndex(Index)) return;

	UShipDataAsset* SelectedData = ShipDataCollection[Index];

	if (SelectedData && PreviewMeshComponent)
	{
		PreviewMeshComponent->SetStaticMesh(SelectedData->DisplayMesh);
		
		OnShipDataChanged.Broadcast(SelectedData);
	}
}