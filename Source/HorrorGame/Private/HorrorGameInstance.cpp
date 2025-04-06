// Fill out your copyright notice in the Description page of Project Settings.

#include "HorrorGameInstance.h"
#include "HorrorSaveGame.h"
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

    const FString SlotName = TEXT("HorrorSaveSlot");

    bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
    if (bSuccess)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Saved: Loop = %d | L1 = %s | L2 = %s | L3 = %s"),
            CurrentLoopIndex,
            bLoop1Complete ? TEXT("true") : TEXT("false"),
            bLoop2Complete ? TEXT("true") : TEXT("false"),
            bLoop3Complete ? TEXT("true") : TEXT("false"));

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Game Saved!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save game to slot!"));
    }
}


bool UHorrorGameInstance::LoadGameProgress()
{
    const FString SlotName = TEXT("HorrorSaveSlot");

    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("No save game found."));
        return false;
    }

    UHorrorSaveGame* LoadedGame = Cast<UHorrorSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

    if (!LoadedGame)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load save game object!"));
        return false;
    }

    // Restore saved values
    CurrentLoopIndex = LoadedGame->SavedLoopIndex;
    bLoop1Complete = LoadedGame->bLoop1Complete;
    bLoop2Complete = LoadedGame->bLoop2Complete;
    bLoop3Complete = LoadedGame->bLoop3Complete;
    bLoop4Complete = LoadedGame->bLoop4Complete;
    bLoop5Complete = LoadedGame->bLoop5Complete;
    bLoop6Complete = LoadedGame->bLoop6Complete;

    UE_LOG(LogTemp, Warning, TEXT("Game Loaded: Loop = %d | L1 = %s | L2 = %s | L3 = %s"),
        CurrentLoopIndex,
        bLoop1Complete ? TEXT("true") : TEXT("false"),
        bLoop2Complete ? TEXT("true") : TEXT("false"),
        bLoop3Complete ? TEXT("true") : TEXT("false"));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Game Loaded!"));
    }

    return true;
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



