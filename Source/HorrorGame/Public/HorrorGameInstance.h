// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "HorrorGameInstance.generated.h"

/**
 * Use this code in your puzzle logic to mark it as complete:
	
	UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GI)
    {
        GI->bHasCompletedPuzzle1 = true;
        UE_LOG(LogTemp, Log, TEXT("Puzzle completed! Exit door now active."));
	}
	
 * This enables the exit door for the loop. replace bHasCompletedPuzzle1 with the correct flag 
 * based on whic puzzle or loop you're working on
 * 
 * 
 * Example use cases:
 * - after viewing a note
 * - after picking up a key/hidden object
 * - after solving a multi-step puzzle
 * 
 * see 'Complete_Loop_Door()' in interaction_System.cpp for door logic
 * 
 */

class UHorrorSaveGame;

UCLASS()
class HORRORGAME_API UHorrorGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// tracks which loop the player is in
	UPROPERTY(BlueprintReadWrite, Category = "Loop")
	int32 CurrentLoopIndex = 1;

	// how many loops are in the game
	UPROPERTY(BlueprintReadWrite, Category = "Loop")
	int32 MaxLoopCount = 6;

	// call this to go to the next loop
	UFUNCTION(BlueprintCallable, Category = "Loop")
	void AdvanceLoop()
	{
		CurrentLoopIndex = FMath::Clamp(CurrentLoopIndex + 1, 1, MaxLoopCount);
	}

	// resets the loop (example: on new game or restart)
	UFUNCTION(BlueprintCallable, Category = "Loop")
	void ResetLoop()
	{
		CurrentLoopIndex = 1;
	}

	// get the current loop index
	UFUNCTION(BlueprintCallable, Category = "Loop")
	int32 GetLoopIndex() const { return CurrentLoopIndex; }

	// per loop puzzle completion flags
	UPROPERTY(BlueprintReadWrite, Category = "PuzzleProgress")
	bool bLoop1Complete = false;

	UPROPERTY(BlueprintReadWrite, Category = "PuzzleProgress")
	bool bLoop2Complete = false;

	UPROPERTY(BlueprintReadWrite, Category = "PuzzleProgress")
	bool bLoop3Complete = false;

	UPROPERTY(BlueprintReadWrite, Category = "PuzzleProgress")
	bool bLoop4Complete = false;

	UPROPERTY(BlueprintReadWrite, Category = "PuzzleProgress")
	bool bLoop5Complete = false;

	UPROPERTY(BlueprintReadWrite, Category = "PuzzleProgress")
	bool bLoop6Complete = false;

	// ...add more for each loop

	// future save game progress implementation
	UFUNCTION(BlueprintCallable)
	void SaveGameProgress();

	UFUNCTION(BlueprintCallable)
	UHorrorSaveGame* LoadGameAndReturn();

	UFUNCTION(BlueprintCallable, Category = "Loop")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Loop", meta = (WorldContext = "WorldContextObject"))
	void ContinueGameWithTransition(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Save")
	bool IsSaveAvailable() const;



	
};
