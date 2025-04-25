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

    if (CurrentLoop != PuzzleLoopIndex)
    {
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        SetActorTickEnabled(false);
        return;
    }

    if (PuzzleTag != NAME_None)
    {
        if (GI->InventoryTags.Contains(PuzzleTag) || GI->HasInteractedWith(PuzzleTag))
        {
            UE_LOG(LogTemp, Warning, TEXT("PuzzleActor '%s' already picked up or interacted with (Tag: %s). Destroying."), *GetName(), *PuzzleTag.ToString());
            Destroy();
            return;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PuzzleActor '%s' has no PuzzleTag set."), *GetName());
    }
}