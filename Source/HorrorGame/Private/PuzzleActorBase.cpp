#include "PuzzleActorBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "HorrorGameInstance.h"

APuzzleActorBase::APuzzleActorBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APuzzleActorBase::BeginPlay()
{
    Super::BeginPlay();

    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GI) return;

    const int32 CurrentLoop = GI->GetLoopIndex();

    if (CurrentLoop != PuzzleLoopIndex)
    {
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        SetActorTickEnabled(false);
        return;
    }

    // restore unlock state if this is the drawer
    if (PuzzleTag == "Room3Drawer" && GI->HasInteractedWith("Room3DrawerUnlocked"))
    {
        bIsUnlocked = true;
        bUnlockedViaPuzzle = true;
    }

    if (PuzzleTag == "PickupFlashlight" || PuzzleTag == "PickupBattery")
    {
        FName LoopAwareTag = FName(*FString::Printf(TEXT("Loop%d_%s"), CurrentLoop, *PuzzleTag.ToString()));
        if (GI->InventoryTags.Contains(LoopAwareTag))
        {
            UE_LOG(LogTemp, Warning, TEXT("Flashlight or battery already picked up for this loop: %s"), *LoopAwareTag.ToString());
            Destroy();
            return;
        }
    }
    else
    {
        // for regular puzzles — remove if previously interacted or picked up
        if (GI->InventoryTags.Contains(PuzzleTag) || GI->HasInteractedWith(PuzzleTag))
        {
            Destroy();
            return;
        }
    }

    if (PuzzleTag == NAME_None)
    {
        UE_LOG(LogTemp, Warning, TEXT("PuzzleActor '%s' has no PuzzleTag set."), *GetName());
    }
}

bool APuzzleActorBase::CanInteract() const
{
    // Special handling for Room3Drawer
    if (PuzzleTag == "Room3Drawer")
    {
        return bIsUnlocked; // Return true after the keypad unlocks it
    }

    // Original logic for other items
    if (PuzzleTag == "PickupFlashlight" || PuzzleTag == "PickupBattery" ||
        PuzzleTag == "Room3Key" || PuzzleTag == "Room2Note")
        return true;

    return bIsUnlocked && bUnlockedViaPuzzle;
}

void APuzzleActorBase::EnableInteraction()
{
    // You can extend this later if needed
    bIsUnlocked = true;
}

void APuzzleActorBase::UnlockSelf_Implementation()
{
    bIsUnlocked = true;
    EnableInteraction();
}