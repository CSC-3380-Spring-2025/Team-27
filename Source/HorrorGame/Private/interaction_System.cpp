// Fill out your copyright notice in the Description page of Project Settings.

#include "interaction_System.h"
#include "Engine/World.h"
#include "first_Person_Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HorrorGameInstance.h"
#include "DrawDebugHelpers.h"
#include "interactable_Data_Table.h"

Ainteraction_System::Ainteraction_System()
{
    PrimaryActorTick.bCanEverTick = true;

    TeleportTargetTag = FName(TEXT("MainRoomSpawn"));
}

void Ainteraction_System::BeginPlay()
{
    Super::BeginPlay();
    InitInteractionFunctionMap();
}

void Ainteraction_System::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Afirst_Person_Character* Character = Cast<Afirst_Person_Character>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (!Character) return;

    FHitResult Hit;
    AActor* Current = LineTraceFromCamera(Character, Hit);

    if (LastHitActor && LastHitActor != Current)
    {
        WidgetPrompt(Character, LastHitActor, false);
    }

    if (Current)
    {
        WidgetPrompt(Character, Current, true);
    }

    LastHitActor = Current;
}

AActor* Ainteraction_System::LineTraceFromCamera(Afirst_Person_Character* Character, FHitResult& Hit)
{
    if (!Character || !Character->cam) return nullptr;

    FVector Start = Character->cam->GetComponentLocation();
    FVector End = Start + (Character->cam->GetForwardVector() * Character->InteractionDistance);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        return Hit.GetActor();
    }

    return nullptr;
}

void Ainteraction_System::InitInteractionFunctionMap()
{
    Interaction_Functions.Add(FName(TEXT("OpenDoor")), &Ainteraction_System::TeleportUsingDataTable);
    Interaction_Functions.Add(FName(TEXT("ExitDoor")), &Ainteraction_System::CompleteLoopDoor);
    Interaction_Functions.Add("TeleportToMainRoom", &Ainteraction_System::TeleportUsingDataTable); // uses the DataTable
    Interaction_Functions.Add(FName(TEXT("Pickup_Object")), &Ainteraction_System::Pickup_Object);
    Interaction_Functions.Add(FName(TEXT("RestoreFlashlightBattery")), &Ainteraction_System::RestoreFlashlightBattery);

}

void Ainteraction_System::Interact(Afirst_Person_Character* Character)
{
    if (!Character || !Character->cam) return;

    FHitResult Hit;
    AActor* HitActor = LineTraceFromCamera(Character, Hit);

    if (HitActor)
    {
        Perform_Interaction(Character, HitActor);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Interact: No valid actor hit."));
        return;
    }
}

void Ainteraction_System::Perform_Interaction(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character) UE_LOG(LogTemp, Error, TEXT("Perform_Interaction failed: Character is nullptr."));
    if (!HitActor) UE_LOG(LogTemp, Error, TEXT("Perform_Interaction failed: HitActor is nullptr."));
    if (!Interaction_Data_Table) UE_LOG(LogTemp, Error, TEXT("Perform_Interaction failed: DataTable is nullptr."));

    if (HitActor->Tags.Contains("LockedDoor"))
    {
        if (Character->AudioComponent)
        {
            Character->AudioComponent->PlayInteractionSound("LockedDoor", HitActor->GetActorLocation());
        }

        UE_LOG(LogTemp, Warning, TEXT("Locked door sound played for actor: %s"), *HitActor->GetName());
        return;
    }

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
            UE_LOG(LogTemp, Log, TEXT("Found function: %s"), *FunctionName.ToString());
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

void Ainteraction_System::WidgetPrompt(Afirst_Person_Character* Character, AActor* HitActor, bool Visibility)
{
    if (!HitActor) return;

    UWidgetComponent* Widget = HitActor->FindComponentByClass<UWidgetComponent>();
    if (Widget)
    {
        FVector WidgetLocation = Widget->GetComponentLocation();
        FVector CameraLocation = Character->cam->GetComponentLocation();

        FRotator WidgetRotation = UKismetMathLibrary::FindLookAtRotation(WidgetLocation, CameraLocation);
        Widget->SetWorldRotation(WidgetRotation);

        Widget->SetVisibility(Visibility);
    }
}


void Ainteraction_System::Pickup_Object(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor) return;

    TSubclassOf<AActor> Item = HitActor->GetClass();
    FName Tag = HitActor->Tags.IsValidIndex(0) ? HitActor->Tags[0] : NAME_None;
    Character->StoredItemScale = HitActor->GetActorScale3D();

    Character->Inventory.Add(Item);
    Character->InventoryTags.Add(Tag);

    Character->CurrentItem = Item;
    Character->CurrentItemTag = Tag;
    Character->CurrentIndex = Character->Inventory.Num() - 1;

    UE_LOG(LogTemp, Log, TEXT("Item '%s' added to inventory."), *HitActor->GetName());

    // if flashlight is picked up, start with 2 battery bars
    if (Tag == "PickupFlashlight")
    {
        // Only set to 2 bars if not previously picked up
        if (!Character->bHasPickedUpFlashlight)
        {
            Character->CurrentBattery = 2;
            Character->bHasPickedUpFlashlight = true;
            UE_LOG(LogTemp, Log, TEXT("First flashlight pickup. Battery set to 2 bars."));
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Flashlight re-picked up. Keeping previous battery level: %d"), Character->CurrentBattery);
        }

        Character->UpdateBatteryUI();
    }

    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character));
    if (GI && HitActor->Tags.Num() > 0)
    {
        GI->MarkTagInteracted(HitActor->Tags[0]);
    }

    HitActor->Destroy();
    Character->AutoTurnOffFlashlight();
}

void Ainteraction_System::TeleportUsingDataTable(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor || !Interaction_Data_Table) return;

    if (Character->AudioComponent)
    {
        Character->AudioComponent->PlayInteractionSound("OpenDoor", HitActor->GetActorLocation());
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

    // Puzzle Completion Check
    switch (CurrentLoop)
    {
    case 1:
        if (Character->CurrentItemTag != "Loop1Key")
        {
            UE_LOG(LogTemp, Warning, TEXT("You must be holding the key to unlock the door."));
            if (Character->AudioComponent)
            {
                Character->AudioComponent->PlayInteractionSound("LockedDoor", HitActor->GetActorLocation());
            }
            return;
        }
        GI->bLoop1Complete = true;
        bCanExit = true;
        break;
    case 2:
    {
        bool bHasNote = Character->InventoryTags.Contains("Room2Note");

        if (bHasNote)
        {
            GI->bLoop2Complete = true;
            bCanExit = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("You must have the Room2Note in your inventory to unlock the door."));
            if (Character->AudioComponent)
            {
                Character->AudioComponent->PlayInteractionSound("LockedDoor", HitActor->GetActorLocation());
            }
            return;
        }
        break;
    }
    case 3: bCanExit = GI->bLoop3Complete; break;
    case 4: bCanExit = GI->bLoop4Complete; break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Unhandled loop index: %d"), CurrentLoop);
        return;
    }

    // generic fallback for incomplete puzzle
    if (!bCanExit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Puzzle for Loop %d is not yet complete."), CurrentLoop);
        if (Character->AudioComponent)
        {
            Character->AudioComponent->PlayInteractionSound("LockedDoor", HitActor->GetActorLocation());
        }
        return;
    }

    // play door open sound
    if (Character->AudioComponent)
    {
        Character->AudioComponent->PlayInteractionSound("ExitDoor", HitActor->GetActorLocation());
    }

    // reset inventory so player doesnt bring stuff into the next loop
    Character->ResetInventory();

    // advance loop
    GI->AdvanceLoop();
    int32 NextLoop = GI->GetLoopIndex();

    FName NextLevelName;
    switch (NextLoop)
    {
    case 2: NextLevelName = FName("Room2"); break;
    case 3: NextLevelName = FName("Room3"); break;
    case 4: NextLevelName = FName("Room4"); break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("No level mapped for Loop %d."), NextLoop);
        return;
    }

    // play the fade transition animation and then load level
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->PlayerCameraManager)
    {
        float FadeDuration = Character->TeleportFadeDuration;
        PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, FadeDuration, FLinearColor::Black, false, true);

        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(DelayHandle, [NextLevelName]()
            {
                UGameplayStatics::OpenLevel(GWorld, NextLevelName);
            }, FadeDuration, false);
    }

    UE_LOG(LogTemp, Warning, TEXT("Loop %d complete. Fading to: %s"), CurrentLoop, *NextLevelName.ToString());
}

void Ainteraction_System::RestoreFlashlightBattery(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor) return;

    // Restore battery to full
    Character->CurrentBattery = Character->MaxBattery;

    // update UI immediately
    Character->UpdateBatteryUI();

    UE_LOG(LogTemp, Warning, TEXT("Battery picked up. Flashlight fully recharged!"));

    HitActor->Destroy();
}

