// Fill out your copyright notice in the Description page of Project Settings.

#include "first_Person_Character.h"
#include "Blueprint/UserWidget.h"
#include "interaction_System.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "HorrorGameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"


/*
    add this wherever a puzzle is triggered/completed (pickup, note, switch, etc)

 * Example: when a puzzle is completed in this class
    
    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GI && GI->GetLoopIndex() == 1)
    {
        GI->bLoop1Complete = true;
        UE_LOG(LogTemp, Log, TEXT("Puzzle for Loop 1 completed!"));
    }

*/

Afirst_Person_Character::Afirst_Person_Character()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Player0;
    bUseControllerRotationYaw = false;

    RootComponent = GetCapsuleComponent();

    cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    cam->SetupAttachment(RootComponent);
    cam->SetRelativeLocation(FVector(0, 0, 40));

    //post process component
    PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
    PostProcessComponent->SetupAttachment(RootComponent);
    PostProcessComponent->bUnbound = true;

    // flashlight component
    Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
    Flashlight->SetupAttachment(cam);
    Flashlight->SetVisibility(false);
    Flashlight->SetIntensity(2500.0f);
    Flashlight->SetInnerConeAngle(20.0f);
    Flashlight->SetOuterConeAngle(35.0f);
    Flashlight->SetAttenuationRadius(1000.0f);
    bFlashlightOn = false;

    //basic movements speeds
    DefaultMaxWalkingSpeed = 150.0f;
    SprintSpeedMultiplier = 1.45f;

    //crouch camera height
    StandingCapsuleHalfHeight = 88.0f;
    CrouchingCapsuleHalfHeight = StandingCapsuleHalfHeight * 0.6f;

    //crouch transition
    MaxCrouchTransitionTime = 0.5f;
    bIsCrouchingInProgress = false;

    //head-bobbing variables
    BobbingTime = 0.0f;
    BobbingSpeed = 10.0f;
    BobbingAmount = 2.5f;
    CrouchBobbingMultiplier = 0.5f;
    bEnableHeadBobbing = true;

    if (cam)
    {
        DefaultCameraPosition = cam->GetRelativeLocation();
        CrouchCameraBaseZ = DefaultCameraPosition.Z;
    }

    //stamina variables
    MaxStamina = 50.0f;
    CurrentStamina = MaxStamina;
    StaminaDrainRate = 20.0f;
    StaminaRegenRate = 5.0f;

    bIsSprinting = false;

    //FOV variables
    DefaultFOV = 90.0f;
    SprintingFOV = 100.0f;
    CrouchFOV = 85.0f;
    FOVTransitionSpeed = 2.0f;

    if (cam)
    {
        cam->SetFieldOfView(DefaultFOV);
    }

    //visual effects defaults
    TargetVignetteIntensity = 0.0f;
    VFXTransitionSpeed = 2.0f;
}

void Afirst_Person_Character::BeginPlay()
{
    Super::BeginPlay();

    CurrentCapsuleHeight = StandingCapsuleHalfHeight;
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCapsuleHeight);
    UpdateMovementSpeed();

    //Spawn an instance of the interaction system class
    Interaction_System = GetWorld()->SpawnActor<Ainteraction_System>(Ainteraction_System::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

    // apply user settings on game start
    if (GEngine && GEngine->GetGameUserSettings())
    {
        GEngine->GetGameUserSettings()->ApplySettings(false); // false = dont restart settings, keep them throughout
    }

    //add crosshair widget to viewpoint
    if (WB_CrosshairClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WB_CrosshairClass);
        if (CrosshairWidget)
        {
            CrosshairWidget->AddToViewport(1);
        }
    }  

    if (BatteryWidgetClass)
    {
        BatteryWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), BatteryWidgetClass);
        if (BatteryWidget)
        {
            BatteryWidget->AddToViewport();
            BatteryWidget->SetVisibility(ESlateVisibility::Hidden); // hides it initially
        }
    }

    // NEW LOOP INIT LOGIC
    UHorrorGameInstance* GameInstance = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GameInstance)
    {
        int32 Loop = GameInstance->GetLoopIndex();
        UE_LOG(LogTemp, Warning, TEXT("Current Loop: %d"), Loop);

        // TODO: Add visual/audio changes based on the loop number
        // if (Loop == 2) { Make hallway darker, flicker lights, etc. }
    }

    // spawn the Blueprint version of the pause manager
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // use LoadClass to find the blueprint class
    UClass* PauseManagerBP = LoadClass<APauseManager>(nullptr, TEXT("/Game/Blueprints/BP_PauseManager.BP_PauseManager_C"));
    PauseManager = PauseManagerBP ? GetWorld()->SpawnActor<APauseManager>(PauseManagerBP, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams) : nullptr;

    // load interaction data table
    CachedInteractionDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/DataTables/InteractableDataTable.InteractableDataTable'"));
    if (Interaction_System && CachedInteractionDataTable)
    {
        Interaction_System->SetInteractionDataTable(CachedInteractionDataTable);
        UE_LOG(LogTemp, Log, TEXT("Interaction Data Table Set Successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load or assign Interaction Data Table!"));
    }
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && PC->PlayerCameraManager)
    {
        FString OptionValue = UGameplayStatics::ParseOption(UGameplayStatics::GetGameMode(this)->OptionsString, TEXT("FadeFromMainMenu"));
        if (OptionValue.Equals("true", ESearchCase::IgnoreCase))
        {
            // start fully black
            PC->PlayerCameraManager->StartCameraFade(1.f, 1.f, 0.f, FLinearColor::Black, true, true);

            // fade out from black after slight delay
            FTimerHandle FadeInHandle;
            GetWorld()->GetTimerManager().SetTimer(FadeInHandle, [PC]()
                {
                    PC->PlayerCameraManager->StartCameraFade(1.f, 0.f, 1.0f, FLinearColor::Black, false, true);
                }, 0.1f, false); // change this so user waits 2 seconds on a black screen 
            UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(this));
            if (GI)
            {
                GI->LoadGameProgress(); // this will auto-apply location + flags
            }
        }
    }
}

void Afirst_Person_Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // adjust FOV based on movement state
    if (bIsSprinting && !bWantsToCrouch && !bIsExhausted)
    {
        CurrentStamina -= StaminaDrainRate * DeltaTime;

        if (CurrentStamina <= 0)
        {
            CurrentStamina = 0;
            StopSprint();
            ApplyStaminaExhaustionEffects();
            bIsExhausted = true;
            GetWorld()->GetTimerManager().SetTimer(ExhaustionTimerHandle, this, &Afirst_Person_Character::ResetExhaustion, ExhaustionRecoveryTime, false);
        }
    }
    // regenerate stamina when not sprinting
    else if (!bIsSprinting && !bIsExhausted && CurrentStamina < MaxStamina)
    {
        CurrentStamina += StaminaRegenRate * DeltaTime;
        if (CurrentStamina > MaxStamina)
        {
            CurrentStamina = MaxStamina;
        }

        // fade the vignette effect when resting
        // a lower transitionspeed means a slower fade
        VFXTransitionSpeed = 0.5f;
        TargetVignetteIntensity = 0.0f;
    }
    else
    {
        //reset transition speed when sprinting
        VFXTransitionSpeed = 2.0f;
    }

    // smooth transition to target FOV
    float NewFOV = FMath::FInterpTo(cam->FieldOfView, GetTargetFOV(), DeltaTime, FOVTransitionSpeed);
    cam->SetFieldOfView(NewFOV);

    ApplyHeadBobbing(DeltaTime);
    SmoothCrouchTransition(DeltaTime);

    if (PostProcessComponent)
    {
        FPostProcessSettings& Settings = PostProcessComponent->Settings;

        Settings.bOverride_VignetteIntensity = true;
        Settings.VignetteIntensity = FMath::FInterpTo(Settings.VignetteIntensity, TargetVignetteIntensity, DeltaTime, VFXTransitionSpeed);
    }

    // BATTERY DRAIN LOGIC
    if (bFlashlightOn)
    {
        BatteryDrainTimer += DeltaTime;

        // Drain every 5 seconds (adjust interval here)
        if (BatteryDrainTimer >= 5.0f)
        {
            BatteryDrainTimer = 0.f;

            if (CurrentBattery > 0)
            {
                CurrentBattery--;

                // Turn off flashlight if empty
                if (CurrentBattery <= 0)
                {
                    CurrentBattery = 0;
                    bFlashlightOn = false;
                    Flashlight->SetVisibility(false);
                    UE_LOG(LogTemp, Warning, TEXT("Battery depleted. Flashlight OFF"));
                }

                UpdateBatteryUI(); // custom function to update widget
            }
        }
    }
}

float Afirst_Person_Character::GetTargetFOV() const
{
    if (bIsSprinting && !bWantsToCrouch && !bIsExhausted)
        return SprintingFOV;
    if (bWantsToCrouch)
        return CrouchFOV;
    return DefaultFOV;
}

void Afirst_Person_Character::UpdateMovementSpeed()
{
    if (bWantsToCrouch)
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed * 0.5f;
    else if (bIsSprinting)
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed * SprintSpeedMultiplier;
    else
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
}

void Afirst_Person_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // CAMERA INPUT (mouse)
    PlayerInputComponent->BindAxis("Horizontal", this, &Afirst_Person_Character::Horizon_Move);
    PlayerInputComponent->BindAxis("Vertical", this, &Afirst_Person_Character::Vertic_Move);
    PlayerInputComponent->BindAxis("SideRotation", this, &Afirst_Person_Character::Horizon_Rot);
    PlayerInputComponent->BindAxis("UpDownRotation", this, &Afirst_Person_Character::Vertic_Rot);

    // SPRINT INPUT (Left Shift)
    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &Afirst_Person_Character::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &Afirst_Person_Character::StopSprint);

    // CROUCH INPUT (Left CTRL)
    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &Afirst_Person_Character::BeginCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &Afirst_Person_Character::EndCrouch);
    
    // INTERACT INPUT (e)
    PlayerInputComponent->BindAction("InteractTest", IE_Pressed, this, &Afirst_Person_Character::Interact);

    // FLASHLIGHT INPUT (f)
    PlayerInputComponent->BindAction("ToggleFlashlight", IE_Pressed, this, &Afirst_Person_Character::ToggleFlashlight);
    PlayerInputComponent->BindAction("DropItem", IE_Pressed, this, &Afirst_Person_Character::DropCurrentItem);
    PlayerInputComponent->BindAxis("ScrollInventory", this, &Afirst_Person_Character::ScrollInventory);

    // PAUSE MENU INPUT (Escape)
    PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &Afirst_Person_Character::TogglePause);
}

void Afirst_Person_Character::Horizon_Move(float value)
{
    if (value)
    {
        AddMovementInput(GetActorRightVector(), value);
    }
}

void Afirst_Person_Character::Vertic_Move(float value)
{
    if (value)
    {
        AddMovementInput(GetActorForwardVector(), value);
    }
}

void Afirst_Person_Character::ToggleFlashlight()
{
    if (CurrentItemTag == "PickupFlashlight")
    {
        if (CurrentBattery > 0)
        {
            bFlashlightOn = !bFlashlightOn;
        }
       
        Flashlight->SetVisibility(bFlashlightOn);

        if (BatteryWidget)
        {
            BatteryWidget->SetVisibility(bFlashlightOn ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }

        if (CurrentBattery <= 0)
        {
            bFlashlightOn = false;
            Flashlight->SetVisibility(false);
            if (BatteryWidget)
            {
                BatteryWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        }
        UpdateBatteryUI();
    }

}

void Afirst_Person_Character::AutoTurnOffFlashlight()
{ 
    if (CurrentItemTag != "PickupFlashlight")
    {
        // Hide battery UI when flashlight is no longer equipped
        if (BatteryWidget)
        {
            BatteryWidget->SetVisibility(ESlateVisibility::Hidden);
        }

        // Also turn flashlight off
        bFlashlightOn = false;
        Flashlight->SetVisibility(false);
        UE_LOG(LogTemp, Log, TEXT("Flashlight forced OFF because it's not being held"));
    }
}

void Afirst_Person_Character::RemoveItemFromInventory()
{
    Inventory.RemoveAt(CurrentIndex);
    InventoryTags.RemoveAt(CurrentIndex);

    if (Inventory.Num() > 0)
    {
        CurrentIndex = CurrentIndex % Inventory.Num();
        CurrentItem = Inventory[CurrentIndex];
        CurrentItemTag = InventoryTags[CurrentIndex];
    }
    else
    {
        CurrentIndex = 0;
        CurrentItem = nullptr;
        CurrentItemTag = NAME_None;
    }
}


void Afirst_Person_Character::UpdateBatteryUI()
{
    if (BatteryWidget)
    {
        UFunction* UpdateFunc = BatteryWidget->FindFunction(FName("UpdateBatteryBars"));
        if (UpdateFunc)
        {
            struct FBatteryParams
            {
                int32 BatteryLevel;
            };

            FBatteryParams Params;
            Params.BatteryLevel = CurrentBattery;

            BatteryWidget->ProcessEvent(UpdateFunc, &Params);
        }
    }
}

void Afirst_Person_Character::DropCurrentItem()
{
    if (!CurrentItem) return;

    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 150.f;
    FRotator SpawnRotation = GetActorRotation();

    UWorld* World = GetWorld();
    if (World)
    {
        FActorSpawnParameters SpawnParams;
        AActor* SpawnedActor = World->SpawnActor<AActor>(CurrentItem, SpawnLocation, SpawnRotation, SpawnParams);
        SpawnedActor->SetActorScale3D(StoredItemScale);

        RemoveItemFromInventory();
        AutoTurnOffFlashlight();
    }
}

void Afirst_Person_Character::ScrollInventory(float AxisValue)
{
    if (Inventory.Num() == 0 || FMath::IsNearlyZero(AxisValue)) return;

    int32 TotalSlots = Inventory.Num() + 1;

    CurrentIndex = (CurrentIndex + (AxisValue > 0 ? 1 : -1) + TotalSlots) % TotalSlots;

    if (CurrentIndex == Inventory.Num())
    {
        CurrentItem = nullptr;
        CurrentItemTag = NAME_None;
        UE_LOG(LogTemp, Log, TEXT("Held item: None"));
    }
    else 
    {
        CurrentItem = (Inventory.IsValidIndex(CurrentIndex)) ? Inventory[CurrentIndex] : nullptr; 
        CurrentItemTag = InventoryTags.IsValidIndex(CurrentIndex) ? InventoryTags[CurrentIndex] : NAME_None;
        if (CurrentItem)
        {
            UE_LOG(LogTemp, Log, TEXT("Held item: %s"), *CurrentItem->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Held item is null (unexpected)"));
        }
    }
    
    AutoTurnOffFlashlight();
}

void Afirst_Person_Character::TogglePause()
{
    if (PauseManager)
    {
        PauseManager->TogglePauseMenu();
    }
}

void Afirst_Person_Character::StartSprint()
{
    if (CurrentStamina > 0 && !bWantsToCrouch && !bIsExhausted)
    {
        bIsSprinting = true;
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed * SprintSpeedMultiplier;
    }
}

void Afirst_Person_Character::StopSprint()
{
    bIsSprinting = false;
    
    if (bWantsToCrouch)
    {
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed * 0.5;
    }
    else
    {
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
    }
}

void Afirst_Person_Character::ApplyHeadBobbing(float DeltaTime) //Head-Bobbing function
{
    if (!bEnableHeadBobbing || !cam) return;

    float Speed = GetVelocity().Size();
    float TargetBobbingFactor = 0.0f;

    // Determine target factor based on state
    if (bIsSprinting)
    {
        TargetBobbingFactor = 1.5f; // intense sprint bob
    }
    else if (Speed > 10.0f)
    {
        TargetBobbingFactor = 0.5f; // subtle walking bob
    }
    else
    {
        TargetBobbingFactor = 0.0f; // idle
    }

    // Smooth interpolation
    CurrentBobbingFactor = FMath::FInterpTo(CurrentBobbingFactor, TargetBobbingFactor, DeltaTime, 5.0f); // tweak 5.0f as smoothing speed

    if (Speed > 10.0f)
    {
        BobbingTime += DeltaTime * BobbingSpeed;
        float BobbingOffset = FMath::Sin(BobbingTime) * BobbingAmount * CurrentBobbingFactor;

        //reduce bob when crouching
        if (bWantsToCrouch)
        {
            BobbingOffset *= CrouchBobbingMultiplier;
        }

        FVector NewCameraPosition = DefaultCameraPosition;
        NewCameraPosition.Z = CrouchCameraBaseZ + BobbingOffset; //apply bob effect

        cam->SetRelativeLocation(NewCameraPosition);
    }
    else
    {
        //reset cam position when standing still
        cam->SetRelativeLocation(FMath::VInterpTo(cam->GetRelativeLocation(), FVector(DefaultCameraPosition.X, DefaultCameraPosition.Y, CrouchCameraBaseZ), DeltaTime, 5.0f));
        BobbingTime = 0.0f;
    }
}

void Afirst_Person_Character::ApplyStaminaExhaustionEffects()
{
    TargetVignetteIntensity = 1.0f;
}

void Afirst_Person_Character::ResetExhaustion()
{
    bIsExhausted = false;
    VFXTransitionSpeed = 0.5f;
}

void Afirst_Person_Character::Horizon_Rot(float value)
{
    if (value)
    {
        AddActorLocalRotation(FRotator(0, value, 0));
    }
}

void Afirst_Person_Character::Vertic_Rot(float value)
{
    if (value)
    {
        float tempo = cam->GetRelativeRotation().Pitch + value;

        if (tempo < 65 && tempo > -65)
        {
            cam->AddLocalRotation(FRotator(value, 0, 0));
        }
    }
}

void Afirst_Person_Character::BeginCrouch()
{
    bWantsToCrouch = true;

    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed * 0.5f;
}

void Afirst_Person_Character::EndCrouch()
{
    bWantsToCrouch = false;

    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
}

void Afirst_Person_Character::SmoothCrouchTransition(float DeltaTime)
{
    float TargetHeight = bWantsToCrouch ? CrouchingCapsuleHalfHeight : StandingCapsuleHalfHeight;

    // interpolate capsule height
    CurrentCapsuleHeight = FMath::FInterpTo(CurrentCapsuleHeight, TargetHeight, DeltaTime, CapsuleInterpSpeed);
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCapsuleHeight, true);

    // interpolate camera position
    float HeightDifference = CurrentCapsuleHeight - StandingCapsuleHalfHeight;
    float TargetCameraZ = DefaultCameraPosition.Z + (HeightDifference * 0.5f);
    CrouchCameraBaseZ = FMath::FInterpTo(CrouchCameraBaseZ, TargetCameraZ, DeltaTime, CapsuleInterpSpeed);
}

void Afirst_Person_Character::Interact()
{
    if (Interaction_System)
    {
        Interaction_System->Interact(this);
    }
}

void Afirst_Person_Character::StartDoorTransition(const FVector& TargetLocation)
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC || !PC->PlayerCameraManager) return;

    // Use configurable property
    const float FadeDuration = TeleportFadeDuration;
    const float TotalTransitionTime = FadeDuration * 2.0f;

    // Fade to black
    PC->PlayerCameraManager->StartCameraFade(0.f, 1.f, FadeDuration, FLinearColor::Black, false, true);

    // Freeze input
    PC->SetIgnoreMoveInput(true);
    PC->SetIgnoreLookInput(true);

    // Create weak pointers for capture safety
    TWeakObjectPtr<Afirst_Person_Character> WeakThis(this);
    TWeakObjectPtr<APlayerController> WeakPC(PC);

    // Teleport after fade-out delay
    FTimerHandle TransitionTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TransitionTimerHandle, [WeakThis, TargetLocation, WeakPC, FadeDuration]()
        {
            if (WeakThis.IsValid() && WeakPC.IsValid() && WeakPC->PlayerCameraManager)
            {
                WeakThis->SetActorLocation(TargetLocation);
                WeakPC->PlayerCameraManager->StartCameraFade(1.f, 0.f, FadeDuration, FLinearColor::Black, false, false);
            }
        }, FadeDuration, false);

    // Unfreeze input after the total transition time
    FTimerHandle InputUnfreezeHandle;
    GetWorld()->GetTimerManager().SetTimer(InputUnfreezeHandle, [WeakPC]()
        {
            if (WeakPC.IsValid())
            {
                WeakPC->SetIgnoreMoveInput(false);
                WeakPC->SetIgnoreLookInput(false);
            }
        }, TotalTransitionTime, false);
}
