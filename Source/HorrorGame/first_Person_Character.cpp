// Fill out your copyright notice in the Description page of Project Settings.

#include "first_Person_Character.h"
#include "DoorTeleport.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

Afirst_Person_Character::Afirst_Person_Character()
{
    PrimaryActorTick.bCanEverTick = true;

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    bUseControllerRotationYaw = false;

    cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    cam->SetupAttachment(RootComponent);
    cam->SetRelativeLocation(FVector(0, 0, 40));

    DefaultMaxWalkingSpeed = 275.0f;
    SprintSpeedMultiplier = 2.0f;
    CrouchSpeed = 150.0f;

    StandingCapsuleHalfHeight = 88.0f;
    CrouchingCapsuleHalfHeight = 44.0f;

    MaxStamina = 100.0f;
    CurrentStamina = MaxStamina;
    StaminaDrainRate = 20.0f;
    StaminaRegenRate = 10.0f;

    bIsCrouching = false;
    bIsSprinting = false;
}

void Afirst_Person_Character::BeginPlay()
{
    Super::BeginPlay();
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
    GetCapsuleComponent()->SetCapsuleHalfHeight(StandingCapsuleHalfHeight);
}

void Afirst_Person_Character::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSprinting && !bIsCrouching)
    {
        CurrentStamina -= StaminaDrainRate * DeltaTime;
        if (CurrentStamina <= 0)
        {
            CurrentStamina = 0;
            StopSprint();
        }
    }
    else
    {
        if (CurrentStamina < MaxStamina)
        {
            CurrentStamina += StaminaRegenRate * DeltaTime;
            if (CurrentStamina > MaxStamina)
            {
                CurrentStamina = MaxStamina;
            }
        }
    }
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

    InputComponent->BindAction("Interact", IE_Pressed, this, &Afirst_Person_Character::InteractWithDoor);
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
    if (CurrentStamina > 0 && !bIsCrouching)
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
    bIsCrouching = true;
    GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchingCapsuleHalfHeight);
    GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void Afirst_Person_Character::EndCrouch()
{
    bIsCrouching = false;
    GetCapsuleComponent()->SetCapsuleHalfHeight(StandingCapsuleHalfHeight);
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
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