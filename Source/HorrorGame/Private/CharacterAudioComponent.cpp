#include "CharacterAudioComponent.h"
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
    UE_LOG(LogTemp, Log, TEXT("[Footstep] Detected surface type: %d"), (int32)SurfaceType);

    FFootstepSounds* CueStruct = FootstepMap.Find(TEnumAsByte<EPhysicalSurface>(SurfaceType));

    if (!CueStruct || CueStruct->Sounds.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Footstep] No cue for surface %d. Trying fallback (SurfaceType_Default = %d)."),
            (int32)SurfaceType, (int32)SurfaceType_Default);

        CueStruct = FootstepMap.Find(TEnumAsByte<EPhysicalSurface>(SurfaceType_Default));

        if (!CueStruct || CueStruct->Sounds.Num() == 0)
        {
            UE_LOG(LogTemp, Error, TEXT("[Footstep] No fallback footstep cue found. Skipping playback."));
            return;
        }
    }

    int32 Index = FMath::RandRange(0, CueStruct->Sounds.Num() - 1);
    if (!CueStruct->Sounds.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Error, TEXT("[Footstep] Invalid cue index %d for surface %d"), Index, (int32)SurfaceType);
        return;
    }

    USoundBase* Cue = CueStruct->Sounds[Index];
    if (!Cue)
    {
        UE_LOG(LogTemp, Error, TEXT("[Footstep] Cue at index %d is null for surface %d"), Index, (int32)SurfaceType);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Footstep] Playing cue: %s | Volume: %.2f | Pitch: %.2f | Surface: %d"),
        *Cue->GetName(), FootstepVolume, PitchMultiplier, (int32)SurfaceType);

    UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(this, Cue, Location, FRotator::ZeroRotator, FootstepVolume, PitchMultiplier);
    if (AudioComp)
    {
        AudioComp->bAutoDestroy = true;
    }
}

void UCharacterAudioComponent::PlayInteractionSound(FName InteractionTag, FVector Location)
{
    if (USoundBase** Found = InteractionMap.Find(InteractionTag))
    {
        UGameplayStatics::PlaySoundAtLocation(this, *Found, Location);
    }
}

void UCharacterAudioComponent::PlayFlashlightToggleSound(bool bTurningOn, FVector Location)
{
    USoundBase* Cue = bTurningOn ? FlashlightOnSound : FlashlightOffSound;

    if (Cue)
    {
        UGameplayStatics::PlaySoundAtLocation(this, Cue, Location);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing flashlight sound for state: %s"), bTurningOn ? TEXT("On") : TEXT("Off"));
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
            return Loaded;
        };

    TArray<USoundBase*> WoodCuesArray = {
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_01_Cue.Parquet_Floor_Mono_01_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_02_Cue.Parquet_Floor_Mono_02_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_03_Cue.Parquet_Floor_Mono_03_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_04_Cue.Parquet_Floor_Mono_04_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Parquet_Floor_Mono_05_Cue.Parquet_Floor_Mono_05_Cue"))
    };

    TArray<USoundBase*> TileCuesArray = {
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Tile_Mono_01_Cue.Tile_Mono_01_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Tile_Mono_02_Cue.Tile_Mono_02_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Tile_Mono_03_Cue.Tile_Mono_03_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Tile_Mono_04_Cue.Tile_Mono_04_Cue")),
        LoadCue(TEXT("/Game/MusicSoundFX/FootstepsMiniPack/SoundCue/Tile_Mono_05_Cue.Tile_Mono_05_Cue"))
    };

    FFootstepSounds WoodCues; WoodCues.Sounds = WoodCuesArray;
    FFootstepSounds TileCues; TileCues.Sounds = TileCuesArray;

    FootstepMap.Add(TEnumAsByte<EPhysicalSurface>(SurfaceType1), WoodCues);
    FootstepMap.Add(TEnumAsByte<EPhysicalSurface>(SurfaceType2), TileCues);
    FootstepMap.Add(TEnumAsByte<EPhysicalSurface>(SurfaceType_Default), WoodCues); // fallback
}

void UCharacterAudioComponent::LoadInteractionSounds()
{
    auto LoadCue = [](const TCHAR* Path) -> USoundBase*
        {
            return Cast<USoundBase>(StaticLoadObject(USoundBase::StaticClass(), nullptr, Path));
        };

    // Door Opening Sound
    InteractionMap.Add("OpenDoor", LoadCue(TEXT("/Game/MusicSoundFX/SmallSoundKit/SSKCue/DoorsCue/Drs_Wood_Door_Open_01_Cue.Drs_Wood_Door_Open_01_Cue")));
    InteractionMap.Add("ExitDoor", LoadCue(TEXT("/Game/MusicSoundFX/SmallSoundKit/SSKCue/DoorsCue/Drs_Wood_Door_Open_01_Cue.Drs_Wood_Door_Open_01_Cue")));

    // Flashlight On/Off Audio
    FlashlightOnSound = LoadCue(TEXT("/Game/MusicSoundFX/FlashlightAudio/FlashlightOnClickAudio_Cue.FlashlightOnClickAudio_Cue"));
    FlashlightOffSound = LoadCue(TEXT("/Game/MusicSoundFX/FlashlightAudio/FlashlightOffClickAudio_Cue.FlashlightOffClickAudio_Cue"));
}
