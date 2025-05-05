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

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
    int32 PuzzleLoopIndex = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
    FName PuzzleTag = NAME_None;

    // Keypad Puzzle Functionality

    UFUNCTION(BlueprintCallable, Category = "Puzzle")
    virtual bool CanInteract() const;

    UFUNCTION(BlueprintCallable, Category = "Puzzle")
    void EnableInteraction(); //unlock the drawer puzzle

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void UnlockSelf();
    virtual void UnlockSelf_Implementation();

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "PuzzleLink")
    AActor* LinkedDrawer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
    bool bUnlockedViaPuzzle = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
    bool bIsUnlocked = true;

private:

    bool bCheckedSaveDestroy = false;

};
