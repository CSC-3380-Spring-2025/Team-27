// Fill out your copyright notice in the Description page of Project Settings.

#include "HorrorGameInstance.h"
#include "HorrorSaveGame.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// all logic in header
// can add more logic here later (example: file saving/loading)

//can move logic like 'AdvanceLoop()' in here for organization/cleanliness

void UHorrorGameInstance::SaveGameProgress()
{
    UHorrorSaveGame* SaveGameInstance = Cast<UHorrorSaveGame>(UGameplayStatics::CreateSaveGameObject(UHorrorSaveGame::StaticClass()));

    if (!SaveGameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create SaveGame object!"));
        return;
    }

    // Save loop index and puzzle flags
    SaveGameInstance->SavedLoopIndex = CurrentLoopIndex;
    SaveGameInstance->bLoop1Complete = bLoop1Complete;
    SaveGameInstance->bLoop2Complete = bLoop2Complete;
    SaveGameInstance->bLoop3Complete = bLoop3Complete;
    SaveGameInstance->bLoop4Complete = bLoop4Complete;
    SaveGameInstance->bLoop5Complete = bLoop5Complete;
    SaveGameInstance->bLoop6Complete = bLoop6Complete;

    // save players position
    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerChar)
    {
        SaveGameInstance->PlayerLocation = PlayerChar->GetActorLocation();
        SaveGameInstance->PlayerRotation = PlayerChar->GetActorRotation();
    }

    const FString SlotName = TEXT("HorrorSaveSlot");
    bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
    if (bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game saved with position: %s | Rotation: %s"),
            *SaveGameInstance->PlayerLocation.ToString(),
            *SaveGameInstance->PlayerRotation.ToString());

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Game Saved!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save game to slot!"));
    }
}


UHorrorSaveGame* UHorrorGameInstance::LoadGameAndReturn()
{
    const FString SlotName = TEXT("HorrorSaveSlot");

    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game found."));
        return nullptr;
    }

    UHorrorSaveGame* LoadedGame = Cast<UHorrorSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
    if (!LoadedGame)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game object!"));
        return nullptr;
    }

    // Restore game instance values
    CurrentLoopIndex = LoadedGame->SavedLoopIndex;
    bLoop1Complete = LoadedGame->bLoop1Complete;
    bLoop2Complete = LoadedGame->bLoop2Complete;
    bLoop3Complete = LoadedGame->bLoop3Complete;
    bLoop4Complete = LoadedGame->bLoop4Complete;
    bLoop5Complete = LoadedGame->bLoop5Complete;
    bLoop6Complete = LoadedGame->bLoop6Complete;

    UE_LOG(LogTemp, Warning, TEXT("Game Loaded: Loop = %d"), CurrentLoopIndex);

    return LoadedGame;
}


void UHorrorGameInstance::StartNewGame()
{
    // reset loop tracking
    CurrentLoopIndex = 1;

    // reset puzzle flags
    bLoop1Complete = false;
    bLoop2Complete = false;
    bLoop3Complete = false;
    bLoop4Complete = false;
    bLoop5Complete = false;
    bLoop6Complete = false;

    // optionally delete the save file (to force a clean state)
    UGameplayStatics::DeleteGameInSlot(TEXT("HorrorSaveSlot"), 0);

    UE_LOG(LogTemp, Warning, TEXT("New Game started. Loop index and puzzle flags reset."));
}

void UHorrorGameInstance::ContinueGameWithTransition(UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid WorldContextObject passed to ContinueGameWithTransition"));
        return;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get World from WorldContextObject"));
        return;
    }

    bLoadFromSaveFile = true;

    if (!LoadGameAndReturn())
    {
        UE_LOG(LogTemp, Warning, TEXT("ContinueGameWithTransition failed: no save found."));
        return;
    }

    // Stop or fade out the menu music (assuming it's tagged as "MenuMusic")
    TArray<AActor*> MusicActors;
    UGameplayStatics::GetAllActorsWithTag(World, TEXT("MenuMusic"), MusicActors);
    for (AActor* Actor : MusicActors)
    {
        UAudioComponent* AudioComp = Actor->FindComponentByClass<UAudioComponent>();
        if (AudioComp)
        {
            // Instantly stop or use fade-out
            // AudioComp->Stop();
            AudioComp->FadeOut(1.2f, 0.0f); // smoother exit
        }
    }

    // Fade to black
    APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
    const float FadeDuration = 1.6f;

    if (PC && PC->PlayerCameraManager)
    {
        PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, FadeDuration, FLinearColor::Black, false, true);
    }

    // Determine level to load
    FString LevelName = FString::Printf(TEXT("Room%d?FadeFromMainMenu=true"), CurrentLoopIndex);

    // Delay load until fade completes
    FTimerHandle LoadLevelTimerHandle;
    World->GetTimerManager().SetTimer(LoadLevelTimerHandle, [LevelName]()
        {
            UGameplayStatics::OpenLevel(GWorld, FName(*LevelName));
        }, FadeDuration + 0.1f, false);
}

bool UHorrorGameInstance::IsSaveAvailable() const
{
    return UGameplayStatics::DoesSaveGameExist(TEXT("HorrorSaveSlot"), 0);
}






