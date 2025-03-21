// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "interactable_Data_Table.h"
#include "interaction_System.generated.h"

class Afirst_Person_Character;

UCLASS()
class HORRORGAME_API Ainteraction_System : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Ainteraction_System();

	void Init(UDataTable* DataTable);
	void Interact(Afirst_Person_Character* Character);

private:
    void Perform_Interaction(const FName Interaction_Function, Afirst_Person_Character* Character);
    void Pickup_Object(Afirst_Person_Character* Character);
    void Open_Door(Afirst_Person_Character* Character);
	void View_Note(Afirst_Person_Character* Character);

    UPROPERTY()
    UDataTable* Interaction_Data_Table;

    TMap<FName, void(Ainteraction_System::*)(Afirst_Person_Character*)> Interaction_System;

};
