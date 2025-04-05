// Fill out your copyright notice in the Description page of Project Settings.

#include "interaction_System.h"
#include "Engine/World.h"
#include "first_Person_Character.h"
#include "HorrorGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "HorrorGameInstance.h"
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
    Interaction_System.Add(FName(TEXT("SolveTestPuzzle")), &Ainteraction_System::Solve_Test_Puzzle);


    Interaction_System.Add(FName(TEXT("CompleteLoopDoor")), &Ainteraction_System::Complete_Loop_Door);  // put after other add calls

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

        //PLACE THE DEBUG LOG RIGHT HERE:
        if (Hit_Actor)
        {
            UE_LOG(LogTemp, Warning, TEXT("Interacted with actor: %s"), *Hit_Actor->GetName());

            if (Hit_Actor->Tags.Num() > 0)
            {
                FName Interaction_ID = Hit_Actor->Tags[0];
                static const FString Context_String(TEXT("Interaction Context"));
                const FInteractable_Data* Interaction_Data = Interaction_Data_Table->FindRow<FInteractable_Data>(Interaction_ID, Context_String);

                if (Interaction_Data)
                {
                    Perform_Interaction(Interaction_Data->Interactable_Function, Character, Hit_Actor);
                }
            }
        }
    }
}

void Ainteraction_System::Perform_Interaction(const FName Interactable_Function, Afirst_Person_Character* Character, AActor* Hit_Actor)
{
    if (Interaction_System.Contains(Interactable_Function))
    {
        (this->*Interaction_System[Interactable_Function])(Character, Hit_Actor);
    }
}

void Ainteraction_System::Pickup_Object(Afirst_Person_Character* Character, AActor* Hit_Actor)
{
    UE_LOG(LogTemp, Log, TEXT("Picked up an object!"));
}

void Ainteraction_System::Open_Door(Afirst_Person_Character* Character, AActor* Hit_Actor)
{
    UE_LOG(LogTemp, Log, TEXT("Opened a door!"));

    float TeleportDistance = 200.0f;
    FVector TeleportOffset = FVector(125.0f, 180.0f, 50.0f);

    FVector TeleportLocation = Hit_Actor->GetActorLocation() + (Hit_Actor->GetActorForwardVector() * TeleportDistance) + TeleportOffset; 

    Character->StartDoorTransition(TeleportLocation);
}

void Ainteraction_System::Complete_Loop_Door(Afirst_Person_Character* Character, AActor* Hit_Actor)
{
    // Safety checks
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character is null."));
        return;
    }

    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character));
    if (!GI)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance is null."));
        return;
    }

    int32 Loop = GI->GetLoopIndex();
    bool bPuzzleComplete = false;

    switch (Loop)
    {
    case 1: bPuzzleComplete = GI->bLoop1Complete; break;
    case 2: bPuzzleComplete = GI->bLoop2Complete; break;
    case 3: bPuzzleComplete = GI->bLoop3Complete; break;
    case 4: bPuzzleComplete = GI->bLoop4Complete; break;
    case 5: bPuzzleComplete = GI->bLoop5Complete; break;
    case 6: bPuzzleComplete = true; break; // Final loop: allow access
    default: bPuzzleComplete = false; break;
    }

    UE_LOG(LogTemp, Warning, TEXT("Loop: %d | Puzzle Complete: %s"), Loop, bPuzzleComplete ? TEXT("true") : TEXT("false"));

    if (!bPuzzleComplete)
    {
        UE_LOG(LogTemp, Warning, TEXT("Puzzle not completed. Door remains locked."));
        return;
    }

    // Teleport player to start location of next loop
    FVector LoopStartLocation = FVector(-3357.690591f, -395.572065f, 92.603541f); // Your actual PlayerStart position
    Character->SetActorLocation(LoopStartLocation);
    UE_LOG(LogTemp, Warning, TEXT("Teleporting player to start location."));

    // Advance to next loop
    GI->AdvanceLoop();
    UE_LOG(LogTemp, Warning, TEXT("Loop %d completed! Now entering Loop %d."), Loop, GI->GetLoopIndex());
}


void Ainteraction_System::View_Note(Afirst_Person_Character* Character, AActor* Hit_Actor)
{
    UE_LOG(LogTemp, Log, TEXT("Viewed a note!"));
}

void Ainteraction_System::Solve_Test_Puzzle(Afirst_Person_Character* Character, AActor* Hit_Actor)
{
    UE_LOG(LogTemp, Log, TEXT("Interacted with TestPuzzle"));

    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character));
    if (GI && GI->GetLoopIndex() == 1)
    {
        GI->bLoop1Complete = true;
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Puzzle marked complete"));
    }

    // Optional: destroy the puzzle object for visual feedback
    if (Hit_Actor)
    {
        Hit_Actor->Destroy();
    }
}
