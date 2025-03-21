// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Blueprint/UserWidget.h"
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

    UPROPERTY(EditAnywhere, Category = "Camera")
    float DefaultFOV; // Default FOV for walking

    UPROPERTY(EditAnywhere, Category = "Camera")
    float SprintingFOV; // Slightly wider FOV when sprinting

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CrouchFOV; // FOV when crouching

    UPROPERTY(EditAnywhere, Category = "Camera")
    float FOVTransitionSpeed;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> WB_CrosshairClass;

    UPROPERTY()
    UUserWidget* CrosshairWidget;

    UPROPERTY(VisibleAnywhere, Category = "PostProcessing")
    UPostProcessComponent* PostProcessComponent;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float DefaultMaxWalkingSpeed;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeedMultiplier;
    bool bIsSprinting;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float CrouchSpeed;

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    bool bEnableHeadBobbing = true; // Toggle head bobbing on/off

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float BobbingSpeed = 10.0f; // Speed of the bobbing effect

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float BobbingAmount = 2.5f; // Intensity of the bobbing effect

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float CrouchBobbingMultiplier = 0.5f; // Reduce bobbing when crouching

    UPROPERTY(VisibleAnywhere, Category = "HeadBobbing")
    float BobbingTime; // Internal timer for bobbing calculation

    UPROPERTY(VisibleAnywhere, Category = "HeadBobbing")
    FVector DefaultCameraPosition; // Stores the default camera position

    void ApplyHeadBobbing(float DeltaTime);

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

    UPROPERTY(VisibleAnywhere, Category = "Stamina")
    bool bIsExhausted = false; //prevents sprinting immediately after running out of stamina

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float ExhaustionRecoveryTime = 2.0f; //time before player can sprint again
    FTimerHandle ExhaustionTimerHandle;
    void ResetExhaustion(); //function to reset exhaustion state

    UPROPERTY(EditAnywhere, Category = "Interaction")
    float InteractionDistance = 200.0f;

    void ApplyStaminaExhaustionEffects();

    void StartSprint();
    void StopSprint();
    void Horizon_Move(float value);
    void Vertic_Move(float value);
    void Horizon_Rot(float value);
    void Vertic_Rot(float value);
    void BeginCrouch();
    void EndCrouch();
    void InteractWithDoor();
};