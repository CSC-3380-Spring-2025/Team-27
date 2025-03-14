// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "first_Person_Character.generated.h"


UCLASS()
class HORRORGAME_API Afirst_Person_Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	Afirst_Person_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private: 

	UPROPERTY(EditAnywhere, Category = "Camera")
		UCameraComponent* cam; 

	UPROPERTY(EditAnywhere, Category = "Movement")
		float DefaultMaxWalkingSpeed;  // Default walking speed

	UPROPERTY(EditAnywhere, Category = "Movement")
		float SprintSpeedMultiplier;     // Sprinting multiplier

	bool bIsSprinting = false;

	void StartSprint();
	void StopSprint();
	
	//Move Left-Right
	void Horizon_Move(float value);

	//Move Forward-Backward
	void Vertic_Move(float value);

	//Look Left-Right
	void Horizon_Rot(float value);

	//Look Up-Down
	void Vertic_Rot(float value); 

};
