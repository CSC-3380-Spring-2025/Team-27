// Fill out your copyright notice in the Description page of Project Settings.

#include "interaction_System.h"
#include "Engine/World.h"
#include "first_Person_Character.h"
#include "HorrorGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "HorrorGameInstance.h"
#include "DrawDebugHelpers.h"
#include "interactable_Data_Table.h"

Ainteraction_System::Ainteraction_System()
{
    PrimaryActorTick.bCanEverTick = false;

    TeleportTargetTag = FName(TEXT("MainRoomSpawn"));
}

void Ainteraction_System::BeginPlay()
{
    Super::BeginPlay();
    InitInteractionFunctionMap();
}

void Ainteraction_System::InitInteractionFunctionMap()
{
    Interaction_Functions.Add(FName(TEXT("OpenDoor")), &Ainteraction_System::TeleportUsingDataTable);
    Interaction_Functions.Add(FName(TEXT("ExitDoor")), &Ainteraction_System::CompleteLoopDoor);
    Interaction_Functions.Add(FName(TEXT("Loop1Key")), &Ainteraction_System::CollectLoop1Key);
    Interaction_Functions.Add("TeleportToMainRoom", &Ainteraction_System::TeleportUsingDataTable); // uses the DataTable

}

void Ainteraction_System::Interact(Afirst_Person_Character* Character)
{
    if (!Character) return;

    FVector Start = Character->cam->GetComponentLocation();
    FVector End = Start + (Character->cam->GetForwardVector() * Character->InteractionDistance); // FIXED: using property, not hardcoded

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        if (Hit.GetActor())
        {
            Perform_Interaction(Character, Hit.GetActor());
        }
    }
}

void Ainteraction_System::Perform_Interaction(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character) UE_LOG(LogTemp, Error, TEXT("Perform_Interaction failed: Character is nullptr."));
    if (!HitActor) UE_LOG(LogTemp, Error, TEXT("Perform_Interaction failed: HitActor is nullptr."));
    if (!Interaction_Data_Table) UE_LOG(LogTemp, Error, TEXT("Perform_Interaction failed: DataTable is nullptr."));


    static const FString ContextString(TEXT("Interaction Lookup"));

    for (const FName& Tag : HitActor->Tags)
    {
        // skips empty tags
        if (Tag.IsNone())
            continue;

        UE_LOG(LogTemp, Log, TEXT("Checking tag: %s"), *Tag.ToString());

        FInteractable_Data* Row = Interaction_Data_Table->FindRow<FInteractable_Data>(Tag, ContextString);
        if (Row)
        {
            const FName& FunctionName = Row->Interactable_Function;

            if (Interaction_Functions.Contains(FunctionName))
            {
                UE_LOG(LogTemp, Log, TEXT("Performing interaction for tag '%s' using function '%s'"), *Tag.ToString(), *FunctionName.ToString());
                (this->*Interaction_Functions[FunctionName])(Character, HitActor);
                return; // success, stop further iteration
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("No function mapped for Interactable_Function: %s"), *FunctionName.ToString());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No data row found for tag: %s"), *Tag.ToString());
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Interaction failed: No matching tag logic found for actor: %s"), *HitActor->GetName());
}

void Ainteraction_System::Pickup_Object(Afirst_Person_Character* Character, AActor* HitActor)
{
    UE_LOG(LogTemp, Log, TEXT("Picked up an object!"));
}

void Ainteraction_System::TeleportUsingDataTable(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor || !Interaction_Data_Table)
    {
        UE_LOG(LogTemp, Error, TEXT("Teleport failed: Missing Character, Actor, or DataTable."));
        return;
    }

    static const FString ContextString(TEXT("Teleport Lookup"));

    for (const FName& Tag : HitActor->Tags)
    {
        if (Tag.IsNone()) continue;

        FInteractable_Data* Row = Interaction_Data_Table->FindRow<FInteractable_Data>(Tag, ContextString);
        if (Row && Row->TeleportTargetTag != NAME_None)
        {
            TArray<AActor*> FoundActors;
            UGameplayStatics::GetAllActorsWithTag(GetWorld(), Row->TeleportTargetTag, FoundActors);

            if (FoundActors.Num() > 0)
            {
                Character->StartDoorTransition(FoundActors[0]->GetActorLocation());
                UE_LOG(LogTemp, Log, TEXT("Teleported to tag '%s' at location %s"),
                    *Row->TeleportTargetTag.ToString(), *FoundActors[0]->GetActorLocation().ToString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("No actor found with tag '%s'"), *Row->TeleportTargetTag.ToString());
            }
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No teleport target found in data table row."));
}


// Exit door logic
void Ainteraction_System::CompleteLoopDoor(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor) return;

    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character));
    if (!GI) return;

    int32 CurrentLoop = GI->GetLoopIndex();
    bool bCanExit = false;

    switch (CurrentLoop)
    {
    case 1: bCanExit = GI->bLoop1Complete; break;
    case 2: bCanExit = GI->bLoop2Complete; break;
    case 3: bCanExit = GI->bLoop3Complete; break;
    }

    if (!bCanExit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Puzzle not completed. Door remains locked."));
        return;
    }

    FVector NewLocation = FVector(-3357.690591f, -395.572065f, 92.603541f); // replace with desired spawn (test), AFTER TEST REPLACE WITH A NEW LEVEL SPAWN FOR EACH LOOP
    Character->SetActorLocation(NewLocation);

    GI->AdvanceLoop();

    UE_LOG(LogTemp, Warning, TEXT("Loop %d completed! Now entering Loop %d."), CurrentLoop, GI->GetLoopIndex());
}


void Ainteraction_System::View_Note(Afirst_Person_Character* Character, AActor* Hit_Actor)
{
    UE_LOG(LogTemp, Log, TEXT("Viewed a note!"));
}

// Key interaction for loop 1
void Ainteraction_System::CollectLoop1Key(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor) return;

    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character));
    if (GI && GI->GetLoopIndex() == 1)
    {
        GI->bLoop1Complete = true;
        HitActor->Destroy();
        UE_LOG(LogTemp, Warning, TEXT("Loop 1 Key Collected!"));
    }
}

