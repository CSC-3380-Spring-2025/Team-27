// Fill out your copyright notice in the Description page of Project Settings.

#include "HorrorGameInstance.h"
#include "HorrorSaveGame.h"
#include "first_Person_Character.h"
#include "Kismet/GameplayStatics.h"

// all logic in header
// can add more logic here later (example: file saving/loading)

//can move logic like 'AdvanceLoop()' in here for organization/cleanliness

void UHorrorGameInstance::SaveGameProgress()
{
    Afirst_Person_Character* Character = Cast<Afirst_Person_Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveGameProgress failed: No valid player character."));
        return;
    }

    UHorrorSaveGame* SaveGameInstance = Cast<UHorrorSaveGame>(UGameplayStatics::CreateSaveGameObject(UHorrorSaveGame::StaticClass()));
    if (!SaveGameInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("SaveGameProgress failed: Couldn't create SaveGame instance."));
        return;
    }

    SaveGameInstance->PlayerLocation = Character->GetActorLocation();
    SaveGameInstance->PlayerRotation = Character->GetActorRotation();
    SaveGameInstance->SavedLoopIndex = CurrentLoopIndex;
    SaveGameInstance->InteractedTags = InteractedTags.Array();
    SaveGameInstance->SavedBatteryLevel = Character->CurrentBattery;
    SaveGameInstance->bHasPickedUpFlashlight = Character->bHasPickedUpFlashlight;

    // Save inventory
    SaveGameInstance->SavedInventory = Character->Inventory;
    SaveGameInstance->SavedInventoryTags = Character->InventoryTags;
    SaveGameInstance->SavedCurrentItem = Character->CurrentItem;
    SaveGameInstance->SavedCurrentItemTag = Character->CurrentItemTag;
    SaveGameInstance->SavedCurrentIndex = Character->CurrentIndex;

    // Save puzzle flags
    SaveGameInstance->bLoop1Complete = bLoop1Complete;
    SaveGameInstance->bLoop2Complete = bLoop2Complete;
    SaveGameInstance->bLoop3Complete = bLoop3Complete;
    SaveGameInstance->bLoop4Complete = bLoop4Complete;
    SaveGameInstance->bLoop5Complete = bLoop5Complete;
    SaveGameInstance->bLoop6Complete = bLoop6Complete;

    // Save to slot
    if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("HorrorSaveSlot"), 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("Game saved successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save game to slot."));
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

    // Restore basic game state
    CurrentLoopIndex = LoadedGame->SavedLoopIndex;
    bLoop1Complete = LoadedGame->bLoop1Complete;
    bLoop2Complete = LoadedGame->bLoop2Complete;
    bLoop3Complete = LoadedGame->bLoop3Complete;
    bLoop4Complete = LoadedGame->bLoop4Complete;
    bLoop5Complete = LoadedGame->bLoop5Complete;
    bLoop6Complete = LoadedGame->bLoop6Complete;
    InteractedTags = TSet<FName>(LoadedGame->InteractedTags);

    UE_LOG(LogTemp, Warning, TEXT("Game Loaded: Loop = %d | L1 = %s | L2 = %s | L3 = %s"),
        CurrentLoopIndex,
        bLoop1Complete ? TEXT("true") : TEXT("false"),
        bLoop2Complete ? TEXT("true") : TEXT("false"),
        bLoop3Complete ? TEXT("true") : TEXT("false"));

    // Restore player state
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        Afirst_Person_Character* Player = Cast<Afirst_Person_Character>(PC->GetPawn());
        if (Player)
        {
            Player->SetActorLocation(LoadedGame->PlayerLocation);
            Player->SetActorRotation(LoadedGame->PlayerRotation);

            Player->CurrentBattery = LoadedGame->SavedBatteryLevel;
            Player->bHasPickedUpFlashlight = LoadedGame->bHasPickedUpFlashlight;
            Player->Inventory = LoadedGame->SavedInventory;
            Player->InventoryTags = LoadedGame->SavedInventoryTags;
            Player->CurrentIndex = LoadedGame->SavedCurrentIndex;
            Player->CurrentItemTag = LoadedGame->SavedCurrentItemTag;
            Player->CurrentItem = LoadedGame->SavedCurrentItem;
            Player->UpdateBatteryUI();

            UE_LOG(LogTemp, Warning, TEXT("Player restored to location: %s"), *LoadedGame->PlayerLocation.ToString());
        }
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

    // clear interacted tags so puzzles reset
    InteractedTags.Empty();
    InventoryTags.Empty();

    // delete save to ensure clean start
    UGameplayStatics::DeleteGameInSlot(TEXT("HorrorSaveSlot"), 0);

    // reset player inventory
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        Afirst_Person_Character* Player = Cast<Afirst_Person_Character>(PC->GetPawn());
        if (Player)
        {
            Player->ResetInventory();
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("New Game started. Loop index, flags, and inventory reset."));
}



