#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlickeringLightComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class YOURGAME_API UFlickeringLightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlickeringLightComponent();

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle FlickerTimerHandle; // Timer for random flickering
	class ULightComponent* LightComponent;

	UPROPERTY(EditAnywhere, Category = "Flicker Settings")
	float FlickerIntensityMin = 200.0f; // Minimum light intensity

	UPROPERTY(EditAnywhere, Category = "Flicker Settings")
	float FlickerIntensityMax = 500.0f; // Maximum light intensity

	UPROPERTY(EditAnywhere, Category = "Flicker Settings")
	float RandomFlickerIntervalMin = 0.05f; // Minimum interval between flickers

	UPROPERTY(EditAnywhere, Category = "Flicker Settings")
	float RandomFlickerIntervalMax = 0.3f; // Maximum interval between flickers

	void TriggerRandomFlicker();
	void ScheduleNextFlicker();
};

