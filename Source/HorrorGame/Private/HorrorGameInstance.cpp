// Fill out your copyright notice in the Description page of Project Settings.

#include "HorrorGameInstance.h"
#include "Kismet/GameplayStatics.h"

// all logic in header
// can add more logic here later (example: file saving/loading)

//can move logic like 'AdvanceLoop()' in here for organization/cleanliness

void UHorrorGameInstance::SaveGameProgress()
{
    // TODO: Implement save logic here
    UE_LOG(LogTemp, Log, TEXT("SaveGameProgress() called."));
}

void UHorrorGameInstance::LoadGameProgress()
{
    // TODO: Implement load logic here
    UE_LOG(LogTemp, Log, TEXT("LoadGameProgress() called."));
}


