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

    int32 CurrentLoop = GI->GetLoopIndex();

    if (CurrentLoop != PuzzleLoopIndex)
    {
        Destroy();
        return;
    }

    bool bComplete = false;
    switch (PuzzleLoopIndex)
    {
    case 1: bComplete = GI->bLoop1Complete; break;
    case 2: bComplete = GI->bLoop2Complete; break;
    case 3: bComplete = GI->bLoop3Complete; break;
    case 4: bComplete = GI->bLoop4Complete; break;
    case 5: bComplete = GI->bLoop5Complete; break;
    case 6: bComplete = GI->bLoop6Complete; break;
    default: break;
    }

    if (bComplete)
    {
        Destroy();
    }
}
