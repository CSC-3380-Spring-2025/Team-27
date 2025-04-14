// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HorrorSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class HORRORGAME_API UHorrorSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
    UPROPERTY()
    int32 SavedLoopIndex;

    UPROPERTY()
    FVector PlayerLocation;

    UPROPERTY()
    FRotator PlayerRotation;

    UPROPERTY()
    bool bLoop1Complete;

    UPROPERTY()
    bool bLoop2Complete;

    UPROPERTY()
    bool bLoop3Complete;

    UPROPERTY()
    bool bLoop4Complete;

    UPROPERTY()
    bool bLoop5Complete;

    UPROPERTY()
    bool bLoop6Complete;
	
};
