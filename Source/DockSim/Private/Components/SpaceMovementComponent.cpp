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

	// 1. A VÁLVULA DE CORTE SECO
	// Se o jogador tirou o dedo do teclado, fecha a válvula instantaneamente.
	if (TargetInput.IsNearlyZero())
	{
		CurrentSmoothedInput = FVector::ZeroVector;
	}
	else
	{
		// 2. A RAMPA DE ACELERAÇÃO
		// Só aplica a rampa se o jogador estiver tentando acelerar para alguma direção.
		CurrentSmoothedInput = FMath::VInterpTo(CurrentSmoothedInput, TargetInput, DeltaTime, LinearInputSmoothing);
	}

	// 3. A FÍSICA E O COMBUSTÍVEL
	// Como a válvula fecha na hora (ZeroVector), este if vai dar FALSO no milissegundo 
	// em que você soltar a tecla, parando 100% o consumo de combustível.
	if (!CurrentSmoothedInput.IsNearlyZero(0.001f))
	{
		if (ResourceComp && ResourceComp->HasFuel())
		{
			// Direção
			FVector MoveDirection = 
				UpdatedMesh->GetForwardVector() * CurrentSmoothedInput.X + 
				UpdatedMesh->GetRightVector()   * CurrentSmoothedInput.Y +
				UpdatedMesh->GetUpVector()      * CurrentSmoothedInput.Z;

			// Multiplicador de Precisão
			float CurrentPrecisionScale = (CurrentPowerMode == EThrustPowerMode::Precision) ? PrecisionModeMultiplier : 1.0f;

			// Força Final
			FVector FinalForce = MoveDirection * ThrustForce * MasterPowerScale * CurrentPrecisionScale;
			UpdatedMesh->AddForce(FinalForce);

			// Consumo de Combustível Proporcional
			float TranslationEffort = FMath::Clamp(CurrentSmoothedInput.Size(), 0.0f, 1.0f);
			float ActualDrain = TranslationEffort * FuelConsumptionRate * CurrentPrecisionScale * DeltaTime;
            
			ResourceComp->ConsumeFuel(ActualDrain);
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
		UpdatedMesh->SetEnableGravity(false); // Estamos no espaço!
		UpdatedMesh->SetLinearDamping(LinearDamping);  // Zero atrito (inércia perfeita do GDD)
	}
	
}

void USpaceMovementComponent::AdjustThrottle(float DeltaAmount)
{
	// Soma o Scroll na potência e trava entre 0.0 (0%) e 1.0 (100%)
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
	// Guardamos a intenção, a mágica física fica no Tick
	TargetInput = TranslationInput;
}

void USpaceMovementComponent::RequestRotation(FVector RotationInput)
{
	if (!UpdatedMesh) return;
	if (ResourceComp && !ResourceComp->HasFuel()) return;

	// 1. Sensibilidade
	FVector ScaledInput = RotationInput * MouseSensitivity;

	// 2. A curva exponencial (DICA: Se ainda der tranco perto do zero, reduza o InputCurveExponent para 1.2 ou 1.5)
	float Pitch = FMath::Sign(ScaledInput.X) * FMath::Pow(FMath::Abs(ScaledInput.X), InputCurveExponent);
	float Yaw   = FMath::Sign(ScaledInput.Y) * FMath::Pow(FMath::Abs(ScaledInput.Y), InputCurveExponent);
	float Roll  = FMath::Sign(ScaledInput.Z) * FMath::Pow(FMath::Abs(ScaledInput.Z), InputCurveExponent);

	FVector TorqueWorld =
	   UpdatedMesh->GetRightVector()   * Pitch +  
	   UpdatedMesh->GetUpVector()      * Yaw +    
	   UpdatedMesh->GetForwardVector() * Roll;    

	FVector InputTorque = TorqueWorld * TorqueForce;

	// 3. A FÍSICA AVELUDADA (bAccelChange)
	// O 'true' no final significa que estamos aplicando aceleração direta, ignorando as falhas da massa!
	UpdatedMesh->AddTorqueInRadians(InputTorque, NAME_None, true);

	// 4. Combustível
	float RawEffort = FVector(Pitch, Yaw, Roll).Size(); 
	float RotationEffort = FMath::Clamp(RawEffort, 0.0f, 1.0f);

	if (RotationEffort > 0.001f)
	{
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		float ActualDrain = RotationEffort * RotationFuelConsumptionRate * DeltaTime; 
		ResourceComp->ConsumeFuel(ActualDrain);
	}
}
