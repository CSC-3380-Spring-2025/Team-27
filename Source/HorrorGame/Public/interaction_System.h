// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "interactable_Data_Table.h"
#include "PuzzleActorBase.h"
#include "Blueprint/UserWidget.h"
#include "interaction_System.generated.h"

class Afirst_Person_Character;
class UWidgetComponent;

UCLASS()
class HORRORGAME_API Ainteraction_System : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Ainteraction_System();

	virtual void Tick(float DeltaTime) override;

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
	void Perform_Interaction(Afirst_Person_Character* Character, AActor* HitActor, const FHitResult& Hit);

	// Pickup interactive objects
	void Pickup_Object(Afirst_Person_Character* Character, AActor* HitActor);

	// function for the Loop Door
	void CompleteLoopDoor(Afirst_Person_Character* Character, AActor* HitActor);

	// function for the flashlight battery
	void RestoreFlashlightBattery(Afirst_Person_Character* Character, AActor* HitActor);

	// calls the teleport location through the data table
	void TeleportUsingDataTable(Afirst_Person_Character* Character, AActor* HitActor);

	// interaction widget 'E' prompt
	void WidgetPrompt(Afirst_Person_Character* Character, AActor* HitActor, bool Visibility);

	// maps the tag function
	TMap<FName, void (Ainteraction_System::*)(Afirst_Person_Character*, AActor*)> Interaction_Functions;
	void InitInteractionFunctionMap();

	// TV Interaction (only plays tv glitch sound for now)
	void GlitchTV(Afirst_Person_Character* Character, AActor* HitActor);

	// keypad puzzle interaction
	void UseKeypad(class Afirst_Person_Character* Character, AActor* HitActor);

	// interactive drawers
	UFUNCTION(BlueprintCallable)
	void UnlockTopDrawer(Afirst_Person_Character* Character, AActor* HitActor);

	AActor* LineTraceFromCamera(Afirst_Person_Character* Character, FHitResult& Hit);
	AActor* LastHitActor = nullptr;

};
