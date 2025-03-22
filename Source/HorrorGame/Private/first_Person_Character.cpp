// Fill out your copyright notice in the Description page of Project Settings.

#include "first_Person_Character.h"
#include "DoorTeleport.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "InGameMenuManager.h"
#include "Blueprint/UserWidget.h"
#include "interaction_System.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"

Afirst_Person_Character::Afirst_Person_Character()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Player0;
    bUseControllerRotationYaw = false;

    cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    cam->SetupAttachment(RootComponent);
    cam->SetRelativeLocation(FVector(0, 0, 40));

    //interaction system initialization
    Interaction_System = CreateDefaultSubobject<Ainteraction_System>(TEXT("InteractionSystem"));

    //post process component
    PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
    PostProcessComponent->SetupAttachment(RootComponent);

    //basic movements speeds
    DefaultMaxWalkingSpeed = 150.0f;
    SprintSpeedMultiplier = 1.4f;
    CrouchSpeed = 75.0f;

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
    }

    //stamina variables
    MaxStamina = 50.0f;
    CurrentStamina = MaxStamina;
    StaminaDrainRate = 20.0f;
    StaminaRegenRate = 5.0f;

    bIsCrouching = false;
    bIsSprinting = false;

    //FOV variables
    DefaultFOV = 90.0f;
    SprintingFOV = 100.0f;
    CrouchFOV = 85.0f;
    FOVTransitionSpeed = 2.5f;

    if (cam)
    {
        cam->SetFieldOfView(DefaultFOV);
    }
}

void Afirst_Person_Character::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
    GetCapsuleComponent()->SetCapsuleHalfHeight(StandingCapsuleHalfHeight);

    //add crosshair to viewpoint
    if (WB_CrosshairClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), WB_CrosshairClass);
        if (CrosshairWidget)
        {
            CrosshairWidget->AddToViewport(1);
        }
    }   

    UDataTable* Interaction_Data_Table = LoadObject<UDataTable>(nullptr, TEXT("/Script/Engine.DataTable'/Game/InteractableDataTable.InteractableDataTable'"));
    if (Interaction_Data_Table)
    {
        Interaction_System->Init(Interaction_Data_Table);
    }
}

void Afirst_Person_Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    float TargetFOV = DefaultFOV;

    // Adjust FOV based on movement state
    if (bIsSprinting && !bIsCrouching && !bIsExhausted)
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

        TargetFOV = SprintingFOV; // Only change FOV when sprinting
    }
    else if (bIsCrouching)
    {
        TargetFOV = CrouchFOV; // Reduce FOV when crouching
    }

    // Regenerate stamina when not sprinting
    if (!bIsSprinting && CurrentStamina < MaxStamina)
    {
        CurrentStamina += StaminaRegenRate * DeltaTime;
        if (CurrentStamina > MaxStamina)
        {
            CurrentStamina = MaxStamina;
        }
    }

    // Smooth transition to target FOV
    float NewFOV = FMath::FInterpTo(cam->FieldOfView, TargetFOV, DeltaTime, FOVTransitionSpeed);
    cam->SetFieldOfView(NewFOV);

    //apply head bobbing
    ApplyHeadBobbing(DeltaTime);
}

void Afirst_Person_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    InputComponent->BindAxis("Horizontal", this, &Afirst_Person_Character::Horizon_Move);
    InputComponent->BindAxis("Vertical", this, &Afirst_Person_Character::Vertic_Move);
    InputComponent->BindAxis("SideRotation", this, &Afirst_Person_Character::Horizon_Rot);
    InputComponent->BindAxis("UpDownRotation", this, &Afirst_Person_Character::Vertic_Rot);

    InputComponent->BindAction("Sprint", IE_Pressed, this, &Afirst_Person_Character::StartSprint);
    InputComponent->BindAction("Sprint", IE_Released, this, &Afirst_Person_Character::StopSprint);

    InputComponent->BindAction("Crouch", IE_Pressed, this, &Afirst_Person_Character::BeginCrouch);
    InputComponent->BindAction("Crouch", IE_Released, this, &Afirst_Person_Character::EndCrouch);
    
    InputComponent->BindAction("InteractTest", IE_Pressed, this, &Afirst_Person_Character::Interact);
    InputComponent->BindAction("Interact", IE_Pressed, this, &Afirst_Person_Character::InteractWithDoor);

    InputComponent->BindAction("ToggleMenu", IE_Pressed, this, &Afirst_Person_Character::ToggleInGameMenu);
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

void Afirst_Person_Character::StartSprint()
{
    if (CurrentStamina > 0 && !bIsCrouching && !bIsExhausted)
    {
        bIsSprinting = true;
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed * SprintSpeedMultiplier;
    }
}

void Afirst_Person_Character::StopSprint()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
}

void Afirst_Person_Character::ApplyHeadBobbing(float DeltaTime) //Head-Bobbing function
{
    if (!bEnableHeadBobbing || !cam) return;

    float Speed = GetVelocity().Size();
    //reduce bob when walking
    float BobbingFactor = 1.0f; //default bobbing factor
    if (Speed > 10.0f && Speed < 150.0f) //bob when walking
    {
        BobbingFactor = 0.5f; //reduce bob intensity when walking
    }
    else if (Speed >= 150.0f) //more bob when sprint
    {
        BobbingFactor = 1.0f;
    }

    if (Speed > 10.0f)
    {
        BobbingTime += DeltaTime * BobbingSpeed;
        float BobbingOffset = FMath::Sin(BobbingTime) * BobbingAmount * BobbingFactor;

        //reduce bob when crouching
        if (bIsCrouching)
        {
            BobbingOffset *= CrouchBobbingMultiplier;
        }

        FVector NewCameraPosition = DefaultCameraPosition;
        NewCameraPosition.Z += BobbingOffset; //apply bob effect

        cam->SetRelativeLocation(NewCameraPosition);
    }
    else
    {
        //reset cam position when standing still
        BobbingTime = 0.0f;
        cam->SetRelativeLocation(FMath::VInterpTo(cam->GetRelativeLocation(), DefaultCameraPosition, DeltaTime, 5.0f));
    }
}

void Afirst_Person_Character::ApplyStaminaExhaustionEffects()
{
    //gradually apply screen distortion each time player runs out of stamina

    if (PostProcessComponent)
    {
        FPostProcessSettings& Settings = PostProcessComponent->Settings;

        Settings.bOverride_VignetteIntensity = true;
        Settings.VignetteIntensity = FMath::Clamp(Settings.VignetteIntensity + 0.2f, 0.0f, 1.0f);

        //add slight blur effect
        Settings.bOverride_BloomIntensity = true;
        Settings.BloomIntensity = FMath::Clamp(Settings.BloomIntensity + 0.05f, 0.0f, 1.0f);
    }
}

void Afirst_Person_Character::ResetExhaustion()
{
    bIsExhausted = false;
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
    if (bIsCrouchingInProgress) return;

    bIsCrouching = true;
    bIsCrouchingInProgress = true;

    InitialCapsuleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    TargetCapsuleHeight = CrouchingCapsuleHalfHeight;
    CurrentCrouchTime = 0.0f;

    GetWorld()->GetTimerManager().SetTimer(CrouchTimerHandle, this, &Afirst_Person_Character::SmoothCrouchTransition, 0.01f, true);
}

void Afirst_Person_Character::EndCrouch()
{
    if (bIsCrouchingInProgress) return;

    bIsCrouching = false;
    bIsCrouchingInProgress = true;

    InitialCapsuleHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    TargetCapsuleHeight = StandingCapsuleHalfHeight;
    CurrentCrouchTime = 0.0f;

    GetWorld()->GetTimerManager().SetTimer(CrouchTimerHandle, this, &Afirst_Person_Character::SmoothCrouchTransition, 0.01f, true);
}

void Afirst_Person_Character::SmoothCrouchTransition()
{
    if (CurrentCrouchTime >= MaxCrouchTransitionTime)
    {
        GetWorld()->GetTimerManager().ClearTimer(CrouchTimerHandle);
        bIsCrouchingInProgress = false;
        return;
    }

    CurrentCrouchTime += 0.01f;

    float Alpha = CurrentCrouchTime / MaxCrouchTransitionTime;
    float NewHeight = FMath::Lerp(InitialCapsuleHeight, TargetCapsuleHeight, Alpha);

    // Clamp the height to prevent overshooting
    NewHeight = FMath::Clamp(NewHeight, FMath::Min(StandingCapsuleHalfHeight, CrouchingCapsuleHalfHeight), FMath::Max(StandingCapsuleHalfHeight, CrouchingCapsuleHalfHeight));
    GetCapsuleComponent()->SetCapsuleHalfHeight(NewHeight, true);

    // Adjust camera smoothly
    FVector CameraLocation = cam->GetRelativeLocation();
    float TargetCameraZ = DefaultCameraPosition.Z + (TargetCapsuleHeight - StandingCapsuleHalfHeight) * 0.5f;
    CameraLocation.Z = FMath::FInterpTo(CameraLocation.Z, TargetCameraZ, 0.01f, 2.0f);
    cam->SetRelativeLocation(CameraLocation);
}

void Afirst_Person_Character::Interact()
{
    if (Interaction_System)
    {
        Interaction_System->Interact(this);
    }
}

void Afirst_Person_Character::InteractWithDoor()
{
    FHitResult HitResult;
    FVector StartLocation = cam->GetComponentLocation();
    FVector EndLocation = StartLocation + (cam->GetForwardVector() * InteractionDistance);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    if (bHit)
    {
        ADoorTeleport* Door = Cast<ADoorTeleport>(HitResult.GetActor());
        if (Door)
        {
            Door->Interact(this);
        }
    }
}

void Afirst_Person_Character::ToggleInGameMenu()
{
    if (InGameMenuWidgetClass)
    {
        if (!InGameMenuWidget)
        {
            InGameMenuWidget = CreateWidget<UInGameMenuManager>(GetWorld(), InGameMenuWidgetClass);
        }

        if (IsValid(InGameMenuWidget)) // Check if the widget is valid
        {
            if (!InGameMenuWidget->IsInViewport())
            {
                InGameMenuWidget->AddToViewport();
                APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                if (IsValid(PlayerController)) // Check if PlayerController is valid
                {
                    PlayerController->SetPause(true);
                    PlayerController->SetInputMode(FInputModeUIOnly());
                    PlayerController->bShowMouseCursor = true;
                }
            }
            else
            {
                InGameMenuWidget->RemoveFromViewport();
                InGameMenuWidget = nullptr; // Reset the widget reference to avoid dangling pointer
                APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                if (IsValid(PlayerController)) // Check if PlayerController is valid
                {
                    PlayerController->SetPause(false);
                    PlayerController->SetInputMode(FInputModeGameOnly());
                    PlayerController->bShowMouseCursor = false;
                }
            }
        }
    }
}