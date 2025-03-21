// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "interaction_System.h"
#include "first_Person_Character.generated.h"

UCLASS()
class HORRORGAME_API Afirst_Person_Character : public ACharacter
{
        GENERATED_BODY()

public:
    Afirst_Person_Character();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
   

private:
    UPROPERTY(EditAnywhere, Category = "Camera")
    UCameraComponent* cam;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float DefaultMaxWalkingSpeed;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeedMultiplier;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float CrouchSpeed;

    UPROPERTY(EditAnywhere, Category = "Crouching")
    float StandingCapsuleHalfHeight;

    UPROPERTY(EditAnywhere, Category = "Crouching")
    float CrouchingCapsuleHalfHeight;

    UPROPERTY(VisibleAnywhere, Category = "Crouching")
    bool bIsCrouching;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float MaxStamina;

    UPROPERTY(VisibleAnywhere, Category = "Stamina")
    float CurrentStamina;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaDrainRate;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaRegenRate;
    bool bIsSprinting;

    UPROPERTY(EditAnywhere, Category = "Interaction")
    float InteractionDistance = 200.0f;

    void StartSprint();
    void StopSprint();
    void Horizon_Move(float value);
    void Vertic_Move(float value);
    void Horizon_Rot(float value);
    void Vertic_Rot(float value);
    void BeginCrouch();
    void EndCrouch();
    void InteractWithDoor();
    void Interact();

    Ainteraction_System* Interaction_System;
};