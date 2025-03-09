// Fill out your copyright notice in the Description page of Project Settings.


#include "first_Person_Character.h"

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
}

// Called when the game starts or when spawned
void Afirst_Person_Character::BeginPlay()
{
	Super::BeginPlay();
	
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

	InputComponent->BindAxis("Horizontal", this, &Afirst_Person_Character::Horizon_Move);
	InputComponent->BindAxis("Vertical", this, &Afirst_Person_Character::Vertic_Move);

	InputComponent->BindAxis("SideRotation", this, &Afirst_Person_Character::Horizon_Rot);
	InputComponent->BindAxis("UpDownRotation", this, &Afirst_Person_Character::Vertic_Rot);
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

void Afirst_Person_Character::Horizon_Rot(float value)
{
	if (value)
	{
		AddActorLocalRotation(FRotator(0,value,0));
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