#include "Components/SpaceMovementComponent.h"
#include "Components/SpaceResourceComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

USpaceMovementComponent::USpaceMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void USpaceMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!UpdatedMesh) return;

	if (TargetInput.IsNearlyZero())
	{
		CurrentSmoothedInput = FVector::ZeroVector;
	}
	else
	{
		CurrentSmoothedInput = FMath::VInterpTo(CurrentSmoothedInput, TargetInput, DeltaTime, LinearInputSmoothing);
	}

	if (!CurrentSmoothedInput.IsNearlyZero(0.001f))
	{
		if (ResourceComp && ResourceComp->HasFuel())
		{
			FVector MoveDirection = 
				UpdatedMesh->GetForwardVector() * CurrentSmoothedInput.X + 
				UpdatedMesh->GetRightVector()   * CurrentSmoothedInput.Y +
				UpdatedMesh->GetUpVector()      * CurrentSmoothedInput.Z;

			float CurrentPrecisionScale = (CurrentPowerMode == EThrustPowerMode::Precision) ? PrecisionModeMultiplier : 1.0f;

			FVector FinalForce = MoveDirection * ThrustForce * MasterPowerScale * CurrentPrecisionScale;
			UpdatedMesh->AddForce(FinalForce);

			float TranslationEffort = FMath::Clamp(CurrentSmoothedInput.Size(), 0.0f, 1.0f);
			float ActualDrain = TranslationEffort * FuelConsumptionRate * CurrentPrecisionScale * DeltaTime;
            
			ResourceComp->ConsumeFuel(ActualDrain);
		}
	}
	if (TargetRotationInput.IsNearlyZero())
	{
		UpdatedMesh->SetAngularDamping(2.0f);
	}
	else
	{
		UpdatedMesh->SetAngularDamping(0.0f);

		if (ResourceComp && ResourceComp->HasFuel())
		{
			FVector ScaledInput = TargetRotationInput * MouseSensitivity;

			float Pitch = FMath::Sign(ScaledInput.X) * FMath::Pow(FMath::Abs(ScaledInput.X), InputCurveExponent);
			float Yaw   = FMath::Sign(ScaledInput.Y) * FMath::Pow(FMath::Abs(ScaledInput.Y), InputCurveExponent);
			float Roll  = FMath::Sign(ScaledInput.Z) * FMath::Pow(FMath::Abs(ScaledInput.Z), InputCurveExponent);

			FVector TorqueWorld =
			   UpdatedMesh->GetRightVector()   * Pitch +  
			   UpdatedMesh->GetUpVector()      * Yaw +    
			   UpdatedMesh->GetForwardVector() * Roll;    
        
			float CurrentPrecisionScale = (CurrentPowerMode == EThrustPowerMode::Precision) ? PrecisionModeMultiplier : 1.0f;
			FVector InputTorque = TorqueWorld * TorqueForce * CurrentPrecisionScale;

			UpdatedMesh->AddTorqueInRadians(InputTorque, NAME_None, true);

			float RawEffort = FVector(Pitch, Yaw, Roll).Size(); 
			float RotationEffort = FMath::Clamp(RawEffort, 0.0f, 1.0f);

			if (RotationEffort > 0.001f)
			{
				float ActualDrain = RotationEffort * RotationFuelConsumptionRate * DeltaTime; 
				ResourceComp->ConsumeFuel(ActualDrain);
			}
		}
	}
}

void USpaceMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UpdatedMesh = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
	ResourceComp = GetOwner()->FindComponentByClass<USpaceResourceComponent>();
	
	if (UpdatedMesh)
	{
		UpdatedMesh->SetSimulatePhysics(true);
		UpdatedMesh->SetEnableGravity(false); 
		UpdatedMesh->SetLinearDamping(LinearDamping); 
		UpdatedMesh->SetAngularDamping(AngularDamping);
	}
	
}

void USpaceMovementComponent::AdjustThrottle(float DeltaAmount)
{
	MasterPowerScale = FMath::Clamp(MasterPowerScale + DeltaAmount, 0.0f, 1.0f);
}

void USpaceMovementComponent::SetThrustPowerMode(EThrustPowerMode NewMode)
{
	if (CurrentPowerMode != NewMode)
	{
		CurrentPowerMode = NewMode;
        
		OnThrustModeChanged.Broadcast(CurrentPowerMode);
	}
}

void USpaceMovementComponent::RequestTranslation(FVector TranslationInput)
{
	TargetInput = TranslationInput;
}

void USpaceMovementComponent::RequestRotation(FVector RotationInput)
{
	TargetRotationInput = RotationInput;
}
