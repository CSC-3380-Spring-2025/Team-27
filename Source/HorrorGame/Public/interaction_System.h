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

	UFUNCTION()
	void Interact(Afirst_Person_Character* Character);

	void SetInteractionDataTable(UDataTable* Table)
	{
		Interaction_Data_Table = Table;
	}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Table")
	UDataTable* Interaction_Data_Table;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	AActor* EntryTeleportTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
	FName TeleportTargetTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float OpenDoorFadeDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float OpenDoorDelayTime = 0.6f;


private:
	// Interaction routing
	void Perform_Interaction(Afirst_Person_Character* Character, AActor* HitActor);

	// Interaction functions
	void Pickup_Object(Afirst_Person_Character* Character, AActor* HitActor);
	void View_Note(Afirst_Person_Character* Character, AActor* HitActor);
	void CollectLoop2Note(Afirst_Person_Character* Character, AActor* HitActor);
	void CollectLoop1Key(Afirst_Person_Character* Character, AActor* HitActor);
	void CompleteLoopDoor(Afirst_Person_Character* Character, AActor* HitActor);

	void TeleportUsingDataTable(Afirst_Person_Character* Character, AActor* HitActor);



	// Map of tag -> function pointer
	TMap<FName, void (Ainteraction_System::*)(Afirst_Person_Character*, AActor*)> Interaction_Functions;

	void InitInteractionFunctionMap();

};
