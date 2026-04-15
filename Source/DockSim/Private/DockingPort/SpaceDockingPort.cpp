#include "DockingPort/SpaceDockingPort.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Player/SpaceShipPawn.h" 

ASpaceDockingPort::ASpaceDockingPort()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. A Raiz (Permite mover o porto inteiro facilmente)
	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	RootComponent = DefaultRoot;

	// 2. A Estrutura Física (O anel/hangar)
	DockStructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DockStructureMesh"));
	DockStructureMesh->SetupAttachment(RootComponent);

	// 3. A Âncora (Onde o bico da nave encaixa - recuado dentro da estrutura)
	DockingAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("DockingAnchor"));
	DockingAnchor->SetupAttachment(DockStructureMesh);

	// 4. A Área de Captura (O "Radar" local da doca)
	CaptureArea = CreateDefaultSubobject<UBoxComponent>(TEXT("CaptureArea"));
	CaptureArea->SetupAttachment(DockStructureMesh);
	
	// Configuração padrão da caixa (pode ser ajustada no Blueprint)
	CaptureArea->SetBoxExtent(FVector(1000.0f, 1000.0f, 1000.0f)); 
	CaptureArea->SetCollisionProfileName(TEXT("Trigger"));
}

void ASpaceDockingPort::BeginPlay()
{
	Super::BeginPlay();

	// Liga os eventos de colisão do Box Component às nossas funções
	CaptureArea->OnComponentBeginOverlap.AddDynamic(this, &ASpaceDockingPort::OnCaptureAreaBeginOverlap);
	CaptureArea->OnComponentEndOverlap.AddDynamic(this, &ASpaceDockingPort::OnCaptureAreaEndOverlap);
}

void ASpaceDockingPort::OnCaptureAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(OtherActor))
	{
		Ship->UpdateDockingSignal(ESignalStatus::Acquired);
	}
}

void ASpaceDockingPort::OnCaptureAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(OtherActor))
	{
		Ship->UpdateDockingSignal(ESignalStatus::Lost);
	}
}