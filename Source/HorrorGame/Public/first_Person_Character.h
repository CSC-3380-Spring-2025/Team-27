// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Blueprint/UserWidget.h"
#include "PauseManager.h"
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
   
    void Interact();

private:
    // FIRST PERSON CAMERA FEATURE
    UPROPERTY(EditAnywhere, Category = "Camera")
    UCameraComponent* cam;

    // FOV CAMERA TRANSITION FEATURE
    UPROPERTY(EditAnywhere, Category = "Camera")
    float DefaultFOV; // default FOV for walking

    UPROPERTY(EditAnywhere, Category = "Camera")
    float SprintingFOV; // slightly wider FOV when sprinting

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CrouchFOV; // FOV when crouching

    UPROPERTY(EditAnywhere, Category = "Camera")
    float FOVTransitionSpeed;

    // CROSSHAIR UI
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> WB_CrosshairClass;

    UPROPERTY()
    UUserWidget* CrosshairWidget;

    // PAUSE MENU UI
    UPROPERTY()
    APauseManager* PauseManager;

    // VISUAL FX FEATURE
    UPROPERTY(VisibleAnywhere, Category = "PostProcessing")
    UPostProcessComponent* PostProcessComponent;

    // PLAYER CONTROLLER FEATURES
    UPROPERTY(EditAnywhere, Category = "Movement")
    float DefaultMaxWalkingSpeed;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float SprintSpeedMultiplier;
    bool bIsSprinting;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float CrouchSpeed;

    // HEAD-BOB FEATURE
    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    bool bEnableHeadBobbing = true; // toggle head bobbing on/off

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float BobbingSpeed = 10.0f; // speed of the bobbing effect

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float BobbingAmount = 2.5f; // intensity of the bobbing effect

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float CrouchBobbingMultiplier = 0.5f; // reduce bobbing when crouching

    UPROPERTY(VisibleAnywhere, Category = "HeadBobbing")
    float BobbingTime; // internal timer for bobbing calculation

    UPROPERTY(VisibleAnywhere, Category = "HeadBobbing")
    FVector DefaultCameraPosition; // stores the default camera position

    UPROPERTY(EditAnywhere, Category = "Crouching")
    float StandingCapsuleHalfHeight;

    UPROPERTY(EditAnywhere, Category = "Crouching")
    float CrouchingCapsuleHalfHeight;

    UPROPERTY(VisibleAnywhere, Category = "Crouching")
    bool bIsCrouching;

    // STAMINA FEATURE
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

    //BASIC INTERACTION FEATURE
    UPROPERTY(EditAnywhere, Category = "Interaction")
    float InteractionDistance = 200.0f;

    // VisualFX FUNCTION DECLARATIONS
    void ApplyStaminaExhaustionEffects();
    void ApplyHeadBobbing(float DeltaTime);

    // SPRINT FUNCTION DECLARATIONS
    void StartSprint();
    void StopSprint();

    // CAMERA FUNCTION DECLARATIONS
    void Horizon_Move(float value);
    void Vertic_Move(float value);
    void Horizon_Rot(float value);
    void Vertic_Rot(float value);

    // PAUSE MENU FUNCTION DECLARATION
    void TogglePause();

    // CROUCH FUNCTION/VARIABLE DECLARATIONS
    void BeginCrouch();
    void EndCrouch();
    void SmoothCrouchTransition();
    FTimerHandle CrouchTimerHandle;
    bool bIsCrouchingInProgress;
    float CurrentCrouchTime;
    float MaxCrouchTransitionTime;
    float InitialCapsuleHeight;
    float TargetCapsuleHeight;

    Ainteraction_System* Interaction_System;
};