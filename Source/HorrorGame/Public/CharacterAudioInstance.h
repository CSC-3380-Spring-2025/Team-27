#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundBase.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CharacterAudioInstance.generated.h"

USTRUCT(BlueprintType)
struct FFootstepSounds
{
    GENERATED_BODY()

    // Mark the TArray with UPROPERTY so that its contents are tracked.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    TArray<USoundBase*> Sounds;
};

UCLASS(ClassGroup = (Audio), meta = (BlueprintSpawnableComponent))
class HORRORGAME_API UCharacterAudioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterAudioComponent();

    void PlayFootstep(EPhysicalSurface SurfaceType, FVector Location, float PitchMultiplier = 1.0f);
    void PlayInteractionSound(FName InteractionTag, FVector Location);

protected:
    virtual void BeginPlay() override;

private:

    UPROPERTY()
    TMap<TEnumAsByte<EPhysicalSurface>, FFootstepSounds> FootstepMap;

    UPROPERTY()
    TMap<FName, USoundBase*> InteractionMap;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float FootstepVolume = 0.5f; // Lower default footstep volume

    void LoadFootstepSounds();
    void LoadInteractionSounds();
};
