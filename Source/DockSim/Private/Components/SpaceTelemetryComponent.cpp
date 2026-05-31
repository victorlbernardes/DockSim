#include "Components/SpaceTelemetryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DockingPort/SpaceDockingPort.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Player/SpaceShipPawn.h"

USpaceTelemetryComponent::USpaceTelemetryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USpaceTelemetryComponent::BeginPlay()
{
	Super::BeginPlay();

	TargetMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
}

float USpaceTelemetryComponent::ApplyPsychologicalZero(float Value) 
{
	const float Threshold = 0.05f;
	return (FMath::Abs(Value) < Threshold) ? 0.0f : Value;
}

FSpaceTelemetryData USpaceTelemetryComponent::GetLatestTelemetry()
{
	FSpaceTelemetryData Data;
	AActor* Owner = GetOwner();
	UPrimitiveComponent* RootMesh = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
	USpaceMovementComponent* MovementComp = Owner->FindComponentByClass<USpaceMovementComponent>();

	if (RootMesh)
	{
		FVector ShipGlobalVelocity = RootMesh->GetPhysicsLinearVelocity();
		FVector TargetGlobalVelocity = FVector::ZeroVector;

		AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceDockingPort::StaticClass());
		if (ASpaceDockingPort* TargetPort = Cast<ASpaceDockingPort>(FoundActor))
		{
			TargetGlobalVelocity = TargetPort->GetRealVelocity();

			USceneComponent* DPoint = nullptr;
			if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(Owner)) 
			{ 
				DPoint = Ship->GetDockingPoint(); 
			}
            
			FVector ShipLoc = DPoint ? DPoint->GetComponentLocation() : Owner->GetActorLocation();
			FVector ShipForward = DPoint ? DPoint->GetForwardVector() : Owner->GetActorForwardVector();
			FVector ShipUp = DPoint ? DPoint->GetUpVector() : Owner->GetActorUpVector();

			FVector PortLoc = TargetPort->DockingAnchor->GetComponentLocation();
			FVector PortForward = TargetPort->DockingAnchor->GetForwardVector();
			FVector PortUp = TargetPort->DockingAnchor->GetUpVector();

			Data.Distance = FVector::Dist(ShipLoc, PortLoc) / 100.0f;

			float RotDot = FVector::DotProduct(ShipForward, -PortForward);
			float RotPercent = FMath::Clamp(RotDot * 100.0f, 0.0f, 100.0f);

			FVector DirToShip = (ShipLoc - PortLoc).GetSafeNormal();
			float PathDot = FVector::DotProduct(PortForward, DirToShip);
			float PathPercent = FMath::Clamp(PathDot * 100.0f, 0.0f, 100.0f);
            
			float UpDot = FVector::DotProduct(ShipUp, PortUp);
			Data.RollAlignmentPercent = FMath::Clamp(UpDot * 100.0f, 0.0f, 100.0f);

			float AlignmentTemp = (RotPercent * PathPercent) / 100.0f;
			Data.NoseAlignmentPercent = AlignmentTemp; 
		}

		Data.VelForward = ApplyPsychologicalZero(FVector::DotProduct(ShipGlobalVelocity, Owner->GetActorForwardVector()) / 100.f);
		Data.VelRight   = ApplyPsychologicalZero(FVector::DotProduct(ShipGlobalVelocity, Owner->GetActorRightVector()) / 100.f);
		Data.VelUp      = ApplyPsychologicalZero(FVector::DotProduct(ShipGlobalVelocity, Owner->GetActorUpVector()) / 100.f);
		Data.VelTotal   = ShipGlobalVelocity.Size() / 100.0f;

		FVector RelativeVelocity = ShipGlobalVelocity - TargetGlobalVelocity;
		
		Data.RelVelForward = ApplyPsychologicalZero(FVector::DotProduct(RelativeVelocity, Owner->GetActorForwardVector()) / 100.f);
		Data.RelVelRight   = ApplyPsychologicalZero(FVector::DotProduct(RelativeVelocity, Owner->GetActorRightVector()) / 100.f);
		Data.RelVelUp      = ApplyPsychologicalZero(FVector::DotProduct(RelativeVelocity, Owner->GetActorUpVector()) / 100.f);
		Data.RelVelTotal   = RelativeVelocity.Size() / 100.0f;

		FRotator CurrentRot = Owner->GetActorRotation();
		Data.Pitch = CurrentRot.Pitch;
		Data.Yaw   = CurrentRot.Yaw;
		Data.Roll  = CurrentRot.Roll;
	}

	if (MovementComp)
	{
		Data.ThrustMode = MovementComp->GetCurrentPowerMode();
		Data.CurrentEngineScale = MovementComp->GetMasterPowerScale();
	}
	
	return Data;
}

bool USpaceTelemetryComponent::IsReadyForHardLock() const
{
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceDockingPort::StaticClass());
    ASpaceDockingPort* TargetPort = Cast<ASpaceDockingPort>(FoundActor);

    if (!TargetPort || !TargetPort->DockingAnchor) return false;

    AActor* Owner = GetOwner(); 
    USceneComponent* DPoint = nullptr;

    if (ASpaceShipPawn* Ship = Cast<ASpaceShipPawn>(Owner))
    {
        DPoint = Ship->GetDockingPoint();
    }

    FVector ShipLoc = DPoint ? DPoint->GetComponentLocation() : Owner->GetActorLocation();
    FVector ShipForward = DPoint ? DPoint->GetForwardVector() : Owner->GetActorForwardVector();
    FVector ShipUp = DPoint ? DPoint->GetUpVector() : Owner->GetActorUpVector();

    FVector PortLoc = TargetPort->DockingAnchor->GetComponentLocation();
    FVector PortForward = TargetPort->DockingAnchor->GetForwardVector();
    FVector PortUp = TargetPort->DockingAnchor->GetUpVector();

    float DistanceMeters = FVector::Dist(ShipLoc, PortLoc) / 100.0f;

    float RotDot = FVector::DotProduct(ShipForward, -PortForward);
    float RotPercent = FMath::Clamp(RotDot * 100.0f, 0.0f, 100.0f);

    FVector DirToShip = (ShipLoc - PortLoc).GetSafeNormal();
    float PathDot = FVector::DotProduct(PortForward, DirToShip);
    float PathPercent = FMath::Clamp(PathDot * 100.0f, 0.0f, 100.0f);

    float UpDot = FVector::DotProduct(ShipUp, PortUp);
    
    float NosePercent = (RotPercent * PathPercent) / 100.0f;
    float RollPercent = FMath::Clamp(UpDot * 100.0f, 0.0f, 100.0f);

    if (DistanceMeters <= 10.0f && NosePercent >= 95.0f && RollPercent >= 95.0f)
    {
        return true;
    }

    return false;
}