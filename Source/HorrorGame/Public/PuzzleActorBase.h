#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleActorBase.generated.h"

UCLASS()
class HORRORGAME_API APuzzleActorBase : public AActor
{
    GENERATED_BODY()

public:
    APuzzleActorBase();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
    int32 PuzzleLoopIndex = 1;

protected:
    virtual void BeginPlay() override;
};
