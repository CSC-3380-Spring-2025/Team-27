#include "PuzzleActorBase.h"
#include "Kismet/GameplayStatics.h"
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

    // Hide the actor if it's not part of this loop
    if (CurrentLoop != PuzzleLoopIndex)
    {
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        SetActorTickEnabled(false);
        UE_LOG(LogTemp, Log, TEXT("PuzzleActor '%s' is hidden (loop %d not active)."), *GetName(), CurrentLoop);
        return;
    }

    // If there's a valid tag and it's already been interacted with, destroy it
    if (Tags.Num() > 0)
    {
        FName Tag = Tags[0];
        if (Tag != NAME_None)
        {
            if (GI->HasInteractedWith(Tag))
            {
                UE_LOG(LogTemp, Warning, TEXT("PuzzleActor '%s' already interacted with (tag: %s). Destroying."),
                    *GetName(), *Tag.ToString());
                Destroy();
                return;
            }

            UE_LOG(LogTemp, Log, TEXT("PuzzleActor '%s' is active with tag '%s' for Loop %d."), *GetName(), *Tag.ToString(), CurrentLoop);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PuzzleActor '%s' has an invalid tag (NAME_None). Will remain active."), *GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PuzzleActor '%s' has no tags set. Will remain active."), *GetName());
    }
}