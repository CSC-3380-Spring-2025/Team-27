// Fill out your copyright notice in the Description page of Project Settings.

#include "interaction_System.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "first_Person_Character.h"
#include "InteractiveAnimationBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "HorrorGameInstance.h"
#include "Misc/OutputDeviceNull.h"
#include "PuzzleActorBase.h"
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
    Interaction_Functions.Add("GlitchTV", &Ainteraction_System::GlitchTV);
    Interaction_Functions.Add("UseKeypad", &Ainteraction_System::UseKeypad);

}

void Ainteraction_System::Interact(Afirst_Person_Character* Character)
{
    if (!Character || !Character->cam) return;

    FHitResult Hit;
    AActor* HitActor = LineTraceFromCamera(Character, Hit);

    if (HitActor)
    {
        Perform_Interaction(Character, HitActor, Hit);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Interact: No valid actor hit."));
        return;
    }
}

void Ainteraction_System::Perform_Interaction(Afirst_Person_Character* Character, AActor* HitActor, const FHitResult& Hit)
{
    if (!HitActor) return;

    // FIRST go through actor tags
    for (const FName& Tag : HitActor->Tags)
    {
        if (Tag == "LockedDoor")
        {
            Character->AudioComponent->PlayInteractionSound("LockedDoor", HitActor->GetActorLocation());
            return;
        }

        if (Interaction_Functions.Contains(Tag))
        {
            (this->*Interaction_Functions[Tag])(Character, HitActor);
            return;
        }
    }

    UPrimitiveComponent* HitComponent = Hit.GetComponent();
    if (HitComponent)
    {
        for (const FName& CompTag : HitComponent->ComponentTags)
        {
            if (CompTag.ToString().StartsWith("Drawer"))
            {
                FName FuncName = FName("OnDrawerInteraction");

                struct FParams
                {
                    UStaticMeshComponent* Drawer;
                    FName DrawerTag;
                } Params;

                Params.Drawer = Cast<UStaticMeshComponent>(HitComponent);
                Params.DrawerTag = CompTag;

                if (Params.Drawer && HitActor->GetClass()->FindFunctionByName(FuncName))
                {
                    HitActor->ProcessEvent(HitActor->FindFunction(FuncName), &Params);
                    UE_LOG(LogTemp, Log, TEXT("Called OnDrawerInteraction for tag: %s"), *CompTag.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to find OnDrawerInteraction or cast drawer"));
                }

                return;
            }
        }
    }

    // fallback to PuzzleActor interaction
    if (APuzzleActorBase* Puzzle = Cast<APuzzleActorBase>(HitActor))
    {
        if (Puzzle->CanInteract())
        {
            Puzzle->UnlockSelf(); // Opens drawer if unlocked
            return;
        }
    }

    for (const FName& Tag : HitActor->Tags)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor Tag: %s"), *Tag.ToString());
    }
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

    // Only auto-equip items that should be in the player's hand
    if (Tag == "PickupFlashlight" || Tag == "Room2Key" || Tag == "Room2Note" || Tag == "Loop1Key")
    {
        Character->CurrentItem = Item;
        Character->CurrentItemTag = Tag;
        Character->CurrentIndex = Character->Inventory.Num() - 1;
        UE_LOG(LogTemp, Log, TEXT("Equipped item: %s"), *Tag.ToString());
    }

    // Handle flashlight pickup
    if (Tag == "PickupFlashlight")
    {
        Character->bHasPickedUpFlashlight = true;

        // If the battery was already picked up, give full charge
        if (Character->bHasPickedUpBattery)
        {
            Character->CurrentBattery = Character->MaxBattery;
            UE_LOG(LogTemp, Log, TEXT("Flashlight picked up after battery. Battery set to max: %d"), Character->CurrentBattery);
        }
        else
        {
            Character->CurrentBattery = 2;
            UE_LOG(LogTemp, Log, TEXT("Flashlight picked up first. Battery set to 2 bars."));
        }

        Character->UpdateBatteryUI();
    }

    // Handle battery pickup
    else if (Tag == "PickupBattery")
    {
        Character->bHasPickedUpBattery = true;

        // If flashlight was already picked up, recharge to full
        if (Character->bHasPickedUpFlashlight && Character->CurrentBattery < Character->MaxBattery)
        {
            Character->CurrentBattery = Character->MaxBattery;
            UE_LOG(LogTemp, Log, TEXT("Battery picked up. Flashlight fully recharged to %d bars."), Character->CurrentBattery);
            Character->UpdateBatteryUI();

            // Refresh flashlight if equipped
            if (Character->CurrentItemTag == "PickupFlashlight")
            {
                Character->Flashlight->SetVisibility(Character->bFlashlightOn);
            }
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Battery picked up before flashlight. Full charge will apply when flashlight is picked up."));
        }
    }

    // Update save data in GameInstance
    if (UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character)))
    {
        int32 CurrentLoop = GI->GetLoopIndex();

        if (Tag == "PickupFlashlight" || Tag == "PickupBattery")
        {
            FName LoopAwareTag = FName(*FString::Printf(TEXT("Loop%d_%s"), CurrentLoop, *Tag.ToString()));
            GI->InventoryTags.Add(LoopAwareTag);
        }
        else
        {
            GI->MarkTagInteracted(Tag);
            GI->InventoryTags.Add(Tag);
        }
    }

    HitActor->Destroy();
    Character->AutoTurnOffFlashlight(); // In case the item just picked up is not the flashlight
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


// Exit/loop door logic
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
        if (GI->InventoryTags.Contains("Room2Note") &&
            GI->InventoryTags.Contains("Room2Key"))
        {
            GI->bLoop2Complete = true;
            bCanExit = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("You must have Room2Note and Room2Key to unlock the door."));
            if (Character->AudioComponent)
            {
                Character->AudioComponent->PlayInteractionSound("LockedDoor", HitActor->GetActorLocation());
            }
            return;
        }
        break;
    case 3: bCanExit = GI->bLoop3Complete; break;
    case 4: bCanExit = GI->bLoop4Complete; break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Unhandled loop index: %d"), CurrentLoop);
        return;
    }

    // fallback if puzzle incomplete
    if (!bCanExit)
    {
        UE_LOG(LogTemp, Warning, TEXT("Puzzle for Loop %d is not yet complete."), CurrentLoop);
        if (Character->AudioComponent)
        {
            Character->AudioComponent->PlayInteractionSound("LockedDoor", HitActor->GetActorLocation());
        }
        return;
    }

    // play open sound
    if (Character->AudioComponent)
    {
        Character->AudioComponent->PlayInteractionSound("ExitDoor", HitActor->GetActorLocation());
    }

    // reset inventory
    Character->ResetInventory();

    FName FlashlightTag = FName(*FString::Printf(TEXT("Loop%d_PickupFlashlight"), CurrentLoop));
    FName BatteryTag = FName(*FString::Printf(TEXT("Loop%d_PickupBattery"), CurrentLoop));
    GI->InventoryTags.Remove(FlashlightTag);
    GI->InventoryTags.Remove(BatteryTag);

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

    // fade and transition
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

    Character->bHasPickedUpBattery = true;

    FName Tag = HitActor->Tags.IsValidIndex(0) ? HitActor->Tags[0] : NAME_None;
    if (Tag != NAME_None)
    {
        UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character));
        if (GI)
        {
            int32 CurrentLoop = GI->GetLoopIndex();
            FName LoopAwareTag = FName(*FString::Printf(TEXT("Loop%d_%s"), CurrentLoop, *Tag.ToString()));
            GI->InventoryTags.Add(LoopAwareTag);
            GI->MarkTagInteracted(Tag);
        }
    }

    if (Character->bHasPickedUpFlashlight)
    {
        Character->CurrentBattery = Character->MaxBattery;
        Character->UpdateBatteryUI();

        // Force flashlight UI/visibility refresh
        if (Character->CurrentItemTag == "PickupFlashlight")
        {
            Character->Flashlight->SetVisibility(Character->bFlashlightOn);
        }

        UE_LOG(LogTemp, Warning, TEXT("Battery picked up. Flashlight fully recharged!"));
    }

    HitActor->Destroy();
}

void Ainteraction_System::UseKeypad(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor) return;

    APlayerController* PC = Cast<APlayerController>(Character->GetController());
    if (!PC) return;

    TSubclassOf<UUserWidget> KeypadClass = LoadClass<UUserWidget>(
        nullptr, TEXT("/Game/UserInterface/WB_KeypadUI.WB_KeypadUI_C"));
    if (!KeypadClass) return;

    UUserWidget* KeypadWidget = CreateWidget<UUserWidget>(PC, KeypadClass);
    if (!KeypadWidget) return;

    // Only set the InteractionSystem reference
    static const FName FuncName2("SetInteractionSystem");
    if (UFunction* Func2 = KeypadWidget->GetClass()->FindFunctionByName(FuncName2))
    {
        struct FSetInteractionSystemParams { Ainteraction_System* System; };
        FSetInteractionSystemParams Params2;
        Params2.System = this;
        KeypadWidget->ProcessEvent(Func2, &Params2);
    }
    // Also set the target drawer actor
    static const FName FuncName("SetTargetDrawer");
    if (UFunction* Func = KeypadWidget->GetClass()->FindFunctionByName(FuncName))
    {
        struct FSetDrawerParams { AActor* Drawer; };
        FSetDrawerParams Params;
        APuzzleActorBase* KeypadPuzzle = Cast<APuzzleActorBase>(HitActor);
        Params.Drawer = KeypadPuzzle ? KeypadPuzzle->LinkedDrawer : nullptr;
        KeypadWidget->ProcessEvent(Func, &Params);
    }

    KeypadWidget->AddToViewport();

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(KeypadWidget->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    PC->SetInputMode(InputMode);
    PC->SetShowMouseCursor(true);
}

void Ainteraction_System::UnlockTopDrawer(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor) return;

    APuzzleActorBase* Drawer = Cast<APuzzleActorBase>(HitActor);
    if (!Drawer)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast %s to PuzzleActorBase"), *HitActor->GetName());
        return;
    }

    Drawer->bUnlockedViaPuzzle = true;
    Drawer->bIsUnlocked = true;
    Drawer->EnableInteraction(); // ensures CanInteract() returns true

    if (UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character)))
    {
        GI->MarkTagInteracted("Room2DrawerUnlocked");
    }

    UE_LOG(LogTemp, Warning, TEXT("Drawer unlocked via keypad: %s"), *Drawer->GetName());
}

// TV interaction (only used for playing a sound, can add more later)
void Ainteraction_System::GlitchTV(Afirst_Person_Character* Character, AActor* HitActor)
{
    if (!Character || !HitActor) return;

    // play the glitch static sound
    if (Character->AudioComponent)
    {
        Character->AudioComponent->PlayInteractionSound("TVGlitch", HitActor->GetActorLocation());
    }
    // mark it as interacted
    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(Character));
    if (GI)
    {
        GI->MarkTagInteracted("Room2TV");
    }

    UE_LOG(LogTemp, Warning, TEXT("TV interacted with. Glitch sound played."));
}

