// Fill out your copyright notice in the Description page of Project Settings.

#include "interaction_System.h"
#include "Engine/World.h"
#include "first_Person_Character.h"
#include "interactable_Data_Table.h"

// Sets default values
Ainteraction_System::Ainteraction_System()
{
}

void Ainteraction_System::Init(UDataTable* DataTable)
{
	Interaction_Data_Table = DataTable;

    //check if datatable is valid
    if (!Interaction_Data_Table)
    {
        UE_LOG(LogTemp, Error, TEXT("Interaction_Data_Table is null!"));
        return; //exit function if datatable is not valid
    }

    //check if datatable is empty
    if (Interaction_Data_Table->GetRowMap().Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Interaction_Data_Table is empty! No interactions available."));
        return; //exit function if datatable is empty
    }

	Interaction_System.Add(FName(TEXT("PickupObject")), &Ainteraction_System::Pickup_Object);
	Interaction_System.Add(FName(TEXT("OpenDoor")), &Ainteraction_System::Open_Door);
	Interaction_System.Add(FName(TEXT("ViewNote")), &Ainteraction_System::View_Note);
}

void Ainteraction_System::Interact(Afirst_Person_Character* Character)
{
    if (!Character || !Interaction_Data_Table) return;

    FVector StartLocation = Character->GetActorLocation();
    FVector EndLocation = StartLocation + (Character->GetActorForwardVector() * 200.0f);

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Character);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    if (bHit)
    {
        AActor* Hit_Actor = HitResult.GetActor();
        if (Hit_Actor && Hit_Actor->Tags.Num() > 0)
        {
            FName Interaction_ID = Hit_Actor->Tags[0];

            static const FString Context_String(TEXT("Interaction Context"));
            const FInteractable_Data* Interaction_Data = Interaction_Data_Table->FindRow<FInteractable_Data>(Interaction_ID, Context_String);

            if (Interaction_Data)
            {
                Perform_Interaction(Interaction_Data->Interactable_Function, Character);
            }
        }
    }
}

void Ainteraction_System::Perform_Interaction(const FName Interactable_Function, Afirst_Person_Character* Character)
{
    if (Interaction_System.Contains(Interactable_Function))
    {
        (this->*Interaction_System[Interactable_Function])(Character);
    }
}

void Ainteraction_System::Pickup_Object(Afirst_Person_Character* Character)
{
    UE_LOG(LogTemp, Log, TEXT("Picked up an object!"));
}

void Ainteraction_System::Open_Door(Afirst_Person_Character* Character)
{
    UE_LOG(LogTemp, Log, TEXT("Opened a door!"));
}

void Ainteraction_System::View_Note(Afirst_Person_Character* Character)
{
    UE_LOG(LogTemp, Log, TEXT("Viewed a note!"));
}





