#include "CharacterAudioInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/AudioComponent.h"
#include "UObject/ConstructorHelpers.h"

UCharacterAudioComponent::UCharacterAudioComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterAudioComponent::BeginPlay()
{
    Super::BeginPlay();
    LoadFootstepSounds();
    LoadInteractionSounds();
}

void UCharacterAudioComponent::PlayFootstep(EPhysicalSurface SurfaceType, FVector Location, float PitchMultiplier)
{
    FFootstepSounds* CueStruct = FootstepMap.Find(TEnumAsByte<EPhysicalSurface>(SurfaceType));

    if (!CueStruct || CueStruct->Sounds.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No footstep cue found for surface type %d. Attempting fallback."), (int32)SurfaceType);
        CueStruct = FootstepMap.Find(TEnumAsByte<EPhysicalSurface>(SurfaceType_Default));
        if (!CueStruct || CueStruct->Sounds.Num() == 0)
        {
            UE_LOG(LogTemp, Error, TEXT("No fallback footstep cue available either. Skipping playback."));
            return;
        }
    }

    int32 Index = FMath::RandRange(0, CueStruct->Sounds.Num() - 1);
    if (!CueStruct->Sounds.IsValidIndex(Index)) return;

    USoundBase* Cue = CueStruct->Sounds[Index];
    if (!Cue) return;

    // Set both pitch and volume here
    UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(this, Cue, Location, FRotator::ZeroRotator, FootstepVolume, PitchMultiplier);
    if (AudioComp)
    {
        AudioComp->bAutoDestroy = true;
        UE_LOG(LogTemp, Log, TEXT("Playing footstep at Volume %.2f | Pitch %.2f | Surface %d"), FootstepVolume, PitchMultiplier, (int32)SurfaceType);
    }
}

void UCharacterAudioComponent::PlayInteractionSound(FName InteractionTag, FVector Location)
{
    if (USoundBase** Found = InteractionMap.Find(InteractionTag))
    {
        UGameplayStatics::PlaySoundAtLocation(this, *Found, Location);
    }
}

void UCharacterAudioComponent::LoadFootstepSounds()
{
    auto LoadCue = [](const TCHAR* Path) -> USoundBase*
        {
            USoundBase* Loaded = Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, Path));
            if (!Loaded)
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load footstep cue at: %s"), Path);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Loaded footstep cue: %s"), *Loaded->GetName());
            }
            return Loaded;
        };

    TArray<USoundBase*> WoodCuesArray = {
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_01_Cue.Parquet_Floor_Mono_01_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_02_Cue.Parquet_Floor_Mono_02_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_03_Cue.Parquet_Floor_Mono_03_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_04_Cue.Parquet_Floor_Mono_04_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_05_Cue.Parquet_Floor_Mono_05_Cue"))
    };

    TArray<USoundBase*> TileCuesArray = {
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Tile_Mono_01_Cue.Tile_Mono_01_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Tile_Mono_02_Cue.Tile_Mono_02_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Tile_Mono_03_Cue.Tile_Mono_03_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Tile_Mono_04_Cue.Tile_Mono_04_Cue")),
        LoadCue(TEXT("/Game/FootstepsMiniPack/SoundCue/Tile_Mono_05_Cue.Tile_Mono_05_Cue"))
    };

    FFootstepSounds WoodCues;
    WoodCues.Sounds = WoodCuesArray;

    FFootstepSounds TileCues;
    TileCues.Sounds = TileCuesArray;

    FootstepMap.Add(TEnumAsByte<EPhysicalSurface>(EPhysicalSurface::SurfaceType1), WoodCues);  // Wood
    FootstepMap.Add(TEnumAsByte<EPhysicalSurface>(EPhysicalSurface::SurfaceType2), TileCues);  // Tile

    // Fallback: using Wood cues as fallback, assuming SurfaceType_Default is defined
    FootstepMap.Add(TEnumAsByte<EPhysicalSurface>(SurfaceType_Default), WoodCues);

    UE_LOG(LogTemp, Warning, TEXT("Footstep sound map initialized with %d Wood cues and %d Tile cues."),
        WoodCuesArray.Num(), TileCuesArray.Num());
}




void UCharacterAudioComponent::LoadInteractionSounds()
{
    auto LoadCue = [](const TCHAR* Path) -> USoundBase*
        {
            return Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, Path));
        };

    // future interaction sounds
    // InteractionMap.Add("Pickup", LoadCue(TEXT("/Game/Audio/Interaction/Pickup_Cue.Pickup_Cue")));
    // InteractionMap.Add("Note", LoadCue(TEXT("/Game/Audio/Interaction/NoteOpen_Cue.NoteOpen_Cue")));
    // InteractionMap.Add("Unlock", LoadCue(TEXT("/Game/Audio/Interaction/DoorUnlock_Cue.DoorUnlock_Cue")));
}

