#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundCue.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "CharacterAudioComponent.generated.h"

// Define a footstep structure
USTRUCT()
struct FFootstepSounds
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<USoundBase*> Sounds;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HORRORGAME_API UCharacterAudioComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterAudioComponent();

    void PlayFootstep(EPhysicalSurface SurfaceType, FVector Location, float PitchMultiplier = 1.0f);
    void PlayInteractionSound(FName InteractionTag, FVector Location);
    void PlayFlashlightToggleSound(bool bTurningOn, FVector Location);

protected:
    virtual void BeginPlay() override;
    void LoadFootstepSounds();
    void LoadInteractionSounds();

    // FLASHLIGHT AUDIO
    UPROPERTY()
    USoundBase* FlashlightOnSound;

    UPROPERTY()
    USoundBase* FlashlightOffSound;

    // Radio Button Audio
    /* Need to add sound files/cue into new content folder; This variable calls to nothing right now. -cliff
    UPROPERTY()
    USoundBase* RadioButtonSound;
    */

    // Footstep Audio
    UPROPERTY()
    TMap<TEnumAsByte<EPhysicalSurface>, FFootstepSounds> FootstepMap;

    UPROPERTY()
    TMap<FName, USoundBase*> InteractionMap;

    float FootstepVolume = 0.2f; // volume multiplier

private:
    static constexpr EPhysicalSurface SurfaceType_Default = SurfaceType1;
};
