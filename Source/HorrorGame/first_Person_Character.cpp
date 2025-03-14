// Fill out your copyright notice in the Description page of Project Settings.


#include "first_Person_Character.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
Afirst_Person_Character::Afirst_Person_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cam->SetupAttachment(RootComponent);
	cam->SetRelativeLocation(FVector(0, 0, 40));

	//sets walk speed and multiplier for speed, once the player clicks shiff, the walk speed is multiplied by this

	DefaultMaxWalkingSpeed = 275.0f;
	SprintSpeedMultiplier = 1.35f;
}

// Called when the game starts or when spawned
void Afirst_Person_Character::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;
	
}

// Called every frame
void Afirst_Person_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void Afirst_Person_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//WASD Movement
	InputComponent->BindAxis("Horizontal", this, &Afirst_Person_Character::Horizon_Move);
	InputComponent->BindAxis("Vertical", this, &Afirst_Person_Character::Vertic_Move);

	//Camera Movement
	InputComponent->BindAxis("SideRotation", this, &Afirst_Person_Character::Horizon_Rot);
	InputComponent->BindAxis("UpDownRotation", this, &Afirst_Person_Character::Vertic_Rot);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &Afirst_Person_Character::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &Afirst_Person_Character::StopSprint);
}

//Move Left-Right
void Afirst_Person_Character::Horizon_Move(float value)
{
	if (value)
	{
		AddMovementInput(GetActorRightVector(), value);
	}
}

//Move Forward-Backward
void Afirst_Person_Character::Vertic_Move(float value)
{
	if (value)
	{
		AddMovementInput(GetActorForwardVector(), value);
	}
}

void Afirst_Person_Character::StartSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed * SprintSpeedMultiplier;
}

void Afirst_Person_Character::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkingSpeed;

}

//Look Left-Right
void Afirst_Person_Character::Horizon_Rot(float value)
{
	if (value)
	{
		AddActorLocalRotation(FRotator(0,value,0));
	}
}

//Look Up-Down
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