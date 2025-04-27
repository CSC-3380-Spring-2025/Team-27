#include "FlickeringLightComponent.h"
#include "Components/LightComponent.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

UFlickeringLightComponent::UFlickeringLightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFlickeringLightComponent::BeginPlay()
{
	Super::BeginPlay();

	LightComponent = GetOwner()->FindComponentByClass<ULightComponent>();
	if (!LightComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Light Component found on %s"), *GetOwner()->GetName());
		return;
	}

	ScheduleNextFlicker();
}

void UFlickeringLightComponent::TriggerRandomFlicker()
{
	if (!LightComponent) return;

	float NewIntensity = FMath::RandRange(FlickerIntensityMin, FlickerIntensityMax);
	LightComponent->SetIntensity(NewIntensity);

	ScheduleNextFlicker();
}

void UFlickeringLightComponent::ScheduleNextFlicker()
{
	if (!GetWorld()) return;

	float NextInterval = FMath::RandRange(RandomFlickerIntervalMin, RandomFlickerIntervalMax);
	GetWorld()->GetTimerManager().SetTimer(FlickerTimerHandle, this, &UFlickeringLightComponent::TriggerRandomFlicker, NextInterval, false);

}

//this one is for random light flickers