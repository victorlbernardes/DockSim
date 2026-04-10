#include "UI/ShipHUDWidget.h"

#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "Components/SpaceTelemetryComponent.h"
#include "Components/SpaceResourceComponent.h"
#include "Kismet/GameplayStatics.h"

void UShipHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// O GetOwningPlayerPawn é a forma mais segura de achar a nave do dono deste HUD
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (PlayerPawn)
	{
		// 1. INSCREVE OS DELEGATES (Eventos)
		if (USpaceMovementComponent* MoveComp = PlayerPawn->FindComponentByClass<USpaceMovementComponent>())
		{
			MoveComp->OnThrustModeChanged.AddDynamic(this, &UShipHUDWidget::HandleThrustModeChanged);
			
			// Força a UI a atualizar o texto assim que a tela abre
			HandleThrustModeChanged(MoveComp->GetCurrentPowerMode());
		}

		if (USpaceResourceComponent* ResourceComp = PlayerPawn->FindComponentByClass<USpaceResourceComponent>())
		{
			ResourceComp->OnFuelEmpty.AddDynamic(this, &UShipHUDWidget::HandleFuelEmpty);
		}

		// 2. CONFIGURA O TIMER DE TELEMETRIA (Fluxo Contínuo)
		CachedTelemetryComp = PlayerPawn->FindComponentByClass<USpaceTelemetryComponent>();
		if (CachedTelemetryComp)
		{
			// Em vez de NativeTick, chamamos a função a cada 0.1 segundos (10 FPS)
			GetWorld()->GetTimerManager().SetTimer(TelemetryTimerHandle, this, &UShipHUDWidget::UpdateContinuousTelemetry, 0.1f, true);
		}
		
		if (IMG_PitchLadder)
		{
			// Isso pega o MI_PitchLadder que colocaremos na Imagem e permite alterar via código
			PitchLadderMID = IMG_PitchLadder->GetDynamicMaterial();
		}
	}
}

void UShipHUDWidget::NativeDestruct()
{
	// Boas Práticas: Sempre destrua o Timer quando fechar o Widget para evitar memory leaks
	GetWorld()->GetTimerManager().ClearTimer(TelemetryTimerHandle);
	Super::NativeDestruct();
}

void UShipHUDWidget::HandleThrustModeChanged(EThrustPowerMode NewMode)
{
	if (!TXT_ThrustMode) return;

	if (NewMode == EThrustPowerMode::Precision)
	{
		TXT_ThrustMode->SetText(FText::FromString(TEXT("PRECISÃO")));
		TXT_ThrustMode->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	else
	{
		TXT_ThrustMode->SetText(FText::FromString(TEXT("STANDARD")));
		TXT_ThrustMode->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	}
}

void UShipHUDWidget::HandleFuelEmpty()
{
	// Dispara o pino vermelho lá no Blueprint Graph!
	OnFuelEmptyAlarm();
}

void UShipHUDWidget::UpdateContinuousTelemetry()
{
	if (!CachedTelemetryComp) return;

	FSpaceTelemetryData Data = CachedTelemetryComp->GetLatestTelemetry();

	// Atualiza Velocidade
	if (TXT_Velocity)
	{
		FString VelString = FString::Printf(TEXT("%.1f m/s"), Data.VelForward);
		TXT_Velocity->SetText(FText::FromString(VelString));
	}

	// Atualiza Combustível
	if (PB_Fuel)
	{
		PB_Fuel->SetPercent(Data.FuelPercent);
	}
	
	if (IMG_Prograde)
	{
		// Sensibilidade: Quantos pixels o ícone move por cada 1 m/s de velocidade lateral/vertical?
		const float PixelPerMeter = 15.0f; 
		
		// O eixo X da tela é a velocidade Direita/Esquerda
		float TranslationX = Data.VelRight * PixelPerMeter;
		
		// O eixo Y da tela é a velocidade Cima/Baixo. 
		// (Atenção: Na Unreal UI, Y positivo é para BAIXO, então invertemos o sinal do VelUp para ficar natural)
		float TranslationY = -Data.VelUp * PixelPerMeter;

		// Limite (Clamp): Para o ícone não sair voando para fora do monitor se você for muito rápido
		const float MaxScreenRadius = 400.0f; 
		TranslationX = FMath::Clamp(TranslationX, -MaxScreenRadius, MaxScreenRadius);
		TranslationY = FMath::Clamp(TranslationY, -MaxScreenRadius, MaxScreenRadius);

		// Aplica o escorregamento na imagem!
		IMG_Prograde->SetRenderTranslation(FVector2D(TranslationX, TranslationY));
		
		// Opcional: Se a nave estiver parada (Inércia Zero), escondemos a bolinha
		if (FMath::IsNearlyZero(Data.VelForward, 0.1f) && FMath::IsNearlyZero(Data.VelRight, 0.1f) && FMath::IsNearlyZero(Data.VelUp, 0.1f))
		{
			IMG_Prograde->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			IMG_Prograde->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
	if (PitchLadderMID)
	{
		// O nó Rotator da Unreal espera um valor de 0 a 1 para uma volta completa (360 graus)
		// Então pegamos o Roll (-180 a 180) e dividimos por 360. Para girar pro lado certo, podemos precisar inverter o sinal (-Data.Roll).
		float NormalizedRoll = -Data.Roll / 360.0f; 

		// O PitchOffset é o quanto a textura "escorrega". 
		// Esse divisor (ex: 90.0f) determina a sensibilidade. Você pode precisar aumentar/diminuir dependendo da sua textura.
		float NormalizedPitch = Data.Pitch / 90.0f; 

		// Envia os números calculados direto para a Placa de Vídeo!
		PitchLadderMID->SetScalarParameterValue(FName("RollAngle"), NormalizedRoll);
		PitchLadderMID->SetScalarParameterValue(FName("PitchOffset"), NormalizedPitch);
	}M_PitchLadder
}