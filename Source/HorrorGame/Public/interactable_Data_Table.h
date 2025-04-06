// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "interactable_Data_Table.generated.h"

USTRUCT(BlueprintType)
struct FInteractable_Data : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Interactable_Function; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TeleportTargetTag;

};