// Fill out your copyright notice in the Description page of Project Settings.

#include "first_Person_Character.h"
#include "Blueprint/UserWidget.h"
#include "interaction_System.h"
#include "GameFramework/GameUserSettings.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "HorrorGameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"


Afirst_Person_Character::Afirst_Person_Character()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Player0;
    bUseControllerRotationYaw = false;

    RootComponent = GetCapsuleComponent();

    cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    cam->SetupAttachment(RootComponent);
    cam->SetRelativeLocation(FVector(0, 0, 40));

    // audio component
    AudioComponent = CreateDefaultSubobject<UCharacterAudioComponent>(TEXT("AudioComponent"));

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
    FOVTransitionSpeed = 1.0f;
    FOVTransitionSpeedRecover = 0.5f;

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

    // load game instance
    UHorrorGameInstance* GameInstance = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GameInstance)
    {
        int32 Loop = GameInstance->GetLoopIndex();
        UE_LOG(LogTemp, Warning, TEXT("Current Loop: %d"), Loop);

        // Only apply default battery if not loading from save
        if (!UGameplayStatics::DoesSaveGameExist(TEXT("HorrorSaveSlot"), 0))
        {
            CurrentBattery = 2;
        }
    }

    CurrentCapsuleHeight = StandingCapsuleHalfHeight;
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCapsuleHeight);
    CurrentMoveSpeed = DefaultMaxWalkingSpeed;
    TargetMoveSpeed = DefaultMaxWalkingSpeed;
    GetCharacterMovement()->MaxWalkSpeed = CurrentMoveSpeed;

    // spawn interaction system
    Interaction_System = GetWorld()->SpawnActor<Ainteraction_System>(Ainteraction_System::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);

    InitializeGraphicsSettings();

    // add crosshair
    if (WB_CrosshairClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WB_CrosshairClass);
        if (CrosshairWidget)
        {
            CrosshairWidget->AddToViewport(1);
        }
    }

    // add battery widget and hide initially
    if (BatteryWidgetClass)
    {
        BatteryWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), BatteryWidgetClass);
        if (BatteryWidget)
        {
            BatteryWidget->AddToViewport();
            BatteryWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // spawn PauseManager
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    UClass* PauseManagerBP = LoadClass<APauseManager>(nullptr, TEXT("/Game/UserInterface/BP_PauseManager.BP_PauseManager_C"));
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

    // Fade from main menu logic
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC && PC->PlayerCameraManager)
    {
        FString OptionValue = UGameplayStatics::ParseOption(UGameplayStatics::GetGameMode(this)->OptionsString, TEXT("FadeFromMainMenu"));

        if (OptionValue.Equals("true", ESearchCase::IgnoreCase))
        {
            PC->PlayerCameraManager->StartCameraFade(1.f, 1.f, 0.f, FLinearColor::Black, true, true);

            TWeakObjectPtr<APlayerController> WeakPC(PC);
            FTimerHandle FadeInHandle;
            GetWorld()->GetTimerManager().SetTimer(FadeInHandle, [WeakPC]()
                {
                    if (WeakPC.IsValid() && WeakPC->PlayerCameraManager)
                    {
                        WeakPC->PlayerCameraManager->StartCameraFade(1.f, 0.f, 1.0f, FLinearColor::Black, false, true);
                    }
                }, 0.1f, false);

            if (GameInstance)
            {
                GameInstance->LoadGameProgress();
            }
        }
    }
}

void Afirst_Person_Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float Speed = GetVelocity().Size();
    bool bIsMoving = Speed > 10.0f;

    float Interval = WalkFootstepInterval;
    if (bIsSprinting) Interval = SprintFootstepInterval;
    else if (bWantsToCrouch) Interval = CrouchFootstepInterval;

    static float PreviousInterval = -1.0f;

    // smoothed out the audio transition between walking, sprinting, and crouching
    if (bIsMoving)
    {
        float NewInterval = WalkFootstepInterval;
        if (bIsSprinting) NewInterval = SprintFootstepInterval;
        else if (bWantsToCrouch) NewInterval = CrouchFootstepInterval;

        if (!GetWorld()->GetTimerManager().IsTimerActive(FootstepTimerHandle) || !FMath::IsNearlyEqual(NewInterval, PreviousInterval, 0.01f))
        {
            // if timer isn't running or interval changed, reset the timer
            GetWorld()->GetTimerManager().ClearTimer(FootstepTimerHandle);
            GetWorld()->GetTimerManager().SetTimer(FootstepTimerHandle, this, &Afirst_Person_Character::PlayFootstep, NewInterval, true);
            PreviousInterval = NewInterval;
        }
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(FootstepTimerHandle);
        PreviousInterval = -1.0f;
    }

    // update target speed based on movement state
    if (bWantsToCrouch)
        TargetMoveSpeed = DefaultMaxWalkingSpeed * 0.5f;
    else if (bIsSprinting && !bIsExhausted)
        TargetMoveSpeed = DefaultMaxWalkingSpeed * SprintSpeedMultiplier;
    else
        TargetMoveSpeed = DefaultMaxWalkingSpeed;

    // smooth interpolate actual move speed
    CurrentMoveSpeed = FMath::FInterpTo(CurrentMoveSpeed, TargetMoveSpeed, DeltaTime, MovementInterpSpeed);
    GetCharacterMovement()->MaxWalkSpeed = CurrentMoveSpeed;

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

    float InterpSpeed = FOVTransitionSpeed;
    if (bIsExhausted && !bIsSprinting)
    {
        InterpSpeed = FOVTransitionSpeedRecover;  // even slower when exhausted
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
        if (BatteryDrainTimer >= 8.0f)
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

void Afirst_Person_Character::InitializeGraphicsSettings()
{
    if (GEngine && GEngine->GetGameUserSettings())
    {
        UGameUserSettings* Settings = GEngine->GetGameUserSettings();

        // this loads previous saved settings
        Settings->LoadSettings();

        // this applies settings without restarting the engine
        Settings->ApplySettings(false);
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
        bool bWasFlashlightOn = bFlashlightOn;

        // only allow toggle if there's battery
        if (CurrentBattery > 0)
        {
            bFlashlightOn = !bFlashlightOn;
        }

        // force flashlight off if battery hits zero
        if (CurrentBattery <= 0)
        {
            bFlashlightOn = false;
        }

        // Update visibility
        Flashlight->SetVisibility(bFlashlightOn);

        // Update UI
        if (BatteryWidget)
        {
            BatteryWidget->SetVisibility(bFlashlightOn ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        }

        // play toggle sound (only if state changed)
        if (bFlashlightOn != bWasFlashlightOn && AudioComponent)
        {
            AudioComponent->PlayFlashlightToggleSound(bFlashlightOn, GetActorLocation());
        }

        // always updates battery UI
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

void Afirst_Person_Character::ApplyHeadBobbing(float DeltaTime)
{
    if (!bEnableHeadBobbing || !cam) return;

    float Speed = GetVelocity().Size();
    float TargetBobbingFactor = 0.0f;
    float TargetBobbingSpeed = 0.0f;
    float TargetBobbingAmount = 0.0f;

    if (bIsSprinting) // sprinting
    {
        TargetBobbingFactor = 1.0f;
        TargetBobbingSpeed = 14.0f;       // faster oscillation
        TargetBobbingAmount = 2.5f;       // stronger bob
    }
    else if (bWantsToCrouch) // crouching
    {
        TargetBobbingFactor = 0.6f;
        TargetBobbingSpeed = 6.0f;        // slower bob
        TargetBobbingAmount = 1.0f;       // subtle crouch bob
    }
    else if (Speed > 10.0f) // walking
    {
        TargetBobbingFactor = 0.8f;
        TargetBobbingSpeed = 10.0f;       // balanced walk speed
        TargetBobbingAmount = 2.0f;
    }

    // smooth transitions between movement states
    CurrentBobbingFactor = FMath::FInterpTo(CurrentBobbingFactor, TargetBobbingFactor, DeltaTime, 6.0f);
    BobbingSpeed = FMath::FInterpTo(BobbingSpeed, TargetBobbingSpeed, DeltaTime, 6.0f);
    BobbingAmount = FMath::FInterpTo(BobbingAmount, TargetBobbingAmount, DeltaTime, 6.0f);

    if (Speed > 10.0f)
    {
        BobbingTime += DeltaTime * BobbingSpeed;
        float BobbingOffset = FMath::Sin(BobbingTime) * BobbingAmount * CurrentBobbingFactor;

        FVector NewCameraPosition = DefaultCameraPosition;
        NewCameraPosition.Z = CrouchCameraBaseZ + BobbingOffset;

        cam->SetRelativeLocation(NewCameraPosition);
    }
    else
    {
        // reset when standing still
        cam->SetRelativeLocation(FMath::VInterpTo(cam->GetRelativeLocation(), FVector(DefaultCameraPosition.X, DefaultCameraPosition.Y, CrouchCameraBaseZ), DeltaTime, 6.0f));
        BobbingTime = 0.0f;
    }
}

void Afirst_Person_Character::ApplyStaminaExhaustionEffects()
{
    TargetVignetteIntensity = 1.0f;

    if (PostProcessComponent)
    {
        auto& Settings = PostProcessComponent->Settings;

        Settings.bOverride_ColorSaturation = true;
        Settings.ColorSaturation = FVector(0.75f); // grayscale look

        Settings.bOverride_SceneFringeIntensity = true;
        Settings.SceneFringeIntensity = 4.0f; // chromatic aberration

        Settings.bOverride_DepthOfFieldFstop = true;
        Settings.DepthOfFieldFstop = 0.5f;
    }
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

void Afirst_Person_Character::PlayFootstep()
{
    // make sure playing is on ground (wood or tile)
    if (!GetCharacterMovement() || !GetCharacterMovement()->IsMovingOnGround())
        return;

    // capsule height for offsetting the trace start
    float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    FVector Start = GetActorLocation() - FVector(0, 0, HalfHeight - 1.f);
    FVector End = Start - FVector(0, 0, FootstepTraceDistance); // how far down we trace

    // setup hit result and trace params
    FHitResult Hit;
    FCollisionQueryParams Params(FName("FootstepTrace"), false, this);
    Params.bReturnPhysicalMaterial = true;

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
        EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PhysMat);

        float Speed = GetVelocity().Size();
        const float WalkSpeed = DefaultMaxWalkingSpeed;
        const float SprintSpeed = WalkSpeed * SprintSpeedMultiplier;
        const float CrouchSpeed = WalkSpeed * 0.5f;

        float Pitch;
        if (Speed >= SprintSpeed * 0.95f)
        {
            Pitch = 1.25f; // sprinting
        }
        else if (Speed <= CrouchSpeed * 1.05f)
        {
            Pitch = 0.85f; // crouching
        }
        else
        {
            Pitch = 1.0f; // walking
        }

        // added slight variation for realism
        Pitch += FMath::RandRange(-0.05f, 0.05f);

        // Play the footstep
        if (AudioComponent)
        {
            AudioComponent->PlayFootstep(SurfaceType, Hit.ImpactPoint, Pitch);
        }

        UE_LOG(LogTemp, Log, TEXT("[Footstep] Hit surface: %d | Material: %s | Actor: %s"),
            (int32)SurfaceType,
            PhysMat ? *PhysMat->GetName() : TEXT("None"),
            Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("None"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Footstep] Line trace missed."));
    }
}

void Afirst_Person_Character::ResetInventory()
{
    Inventory.Empty();
    InventoryTags.Empty();
    CurrentItem = nullptr;
    CurrentItemTag = NAME_None;
    CurrentIndex = 0;
    bHasPickedUpFlashlight = false;
    CurrentBattery = 0;
    AutoTurnOffFlashlight();
    UpdateBatteryUI();

    UE_LOG(LogTemp, Warning, TEXT("Inventory reset due to loop transition or new game."));
}
