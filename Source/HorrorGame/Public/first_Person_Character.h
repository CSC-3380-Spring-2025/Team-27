// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/SpotLightComponent.h"
#include "Blueprint/UserWidget.h"
#include "PauseManager.h"
#include "CharacterAudioComponent.h"
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

    UPROPERTY()
    UDataTable* CachedInteractionDataTable;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    void InitializeGraphicsSettings();

    void StartDoorTransition(const FVector& TargetLocation);
   
    void Interact();
    void StartSprint();
    void StopSprint();
    void BeginCrouch();
    void EndCrouch();
    void TogglePause();

    // FIRST PERSON CAMERA FEATURE
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    UCameraComponent* cam;

    // AUDIO FEATURES
    UPROPERTY()
    UCharacterAudioComponent* AudioComponent;

    UFUNCTION()
    void PlayFootstep();

    //BASIC INTERACTION FEATURE
    UPROPERTY(EditAnywhere, Category = "Interaction")
    float InteractionDistance = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float TeleportFadeDuration = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
    float TeleportDelayTime = 0.6f;

    // FLASHLIGHT FEATURE
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USpotLightComponent* Flashlight;

    UPROPERTY(BlueprintReadWrite)
    bool bHasPickedUpBattery = false;

    UPROPERTY(BlueprintReadWrite)
    bool bHasPickedUpFlashlight = false;

    bool bFlashlightOn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight")
    int32 MaxBattery = 5;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Flashlight")
    int32 CurrentBattery = 5;

    UFUNCTION()
    void UpdateBatteryUI();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> BatteryWidgetClass;

    UUserWidget* BatteryWidget;

    // INVENTORY SYSTEM
    UPROPERTY()
    TArray<TSubclassOf<AActor>> Inventory;

    UPROPERTY()
    TArray<FName> InventoryTags;
    
    FName CurrentItemTag = NAME_None;
    TSubclassOf<AActor> CurrentItem = nullptr;
    int32 CurrentIndex = 0;

    // RESET INVENTORY WHEN LOOP/START NEW GAME
    UFUNCTION()
    void ResetInventory();
    void ToggleFlashlight();
    void AutoTurnOffFlashlight();
    void ScrollInventory(float Value);

private:
    // FOV CAMERA TRANSITION FEATURE
    UPROPERTY(EditAnywhere, Category = "Camera")
    float DefaultFOV;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float SprintingFOV;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float CrouchFOV;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float FOVTransitionSpeed;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float FOVTransitionSpeedRecover = 1.0f;

    // AUDIO FEATURE
    UPROPERTY(EditAnywhere, Category = "Audio")
    float FootstepTraceDistance = 100.f;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float WalkFootstepInterval = 0.6f;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float SprintFootstepInterval = 0.45f;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float CrouchFootstepInterval = 0.9f;

    FTimerHandle FootstepTimerHandle;

    // CROSSHAIR UI
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> WB_CrosshairClass;

    UPROPERTY()
    UUserWidget* CrosshairWidget;

    // flashlight drain
    float BatteryDrainTimer = 0.f;

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

    // HEAD-BOB FEATURE
    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    bool bEnableHeadBobbing = true; // toggle head bobbing on/off

    float CurrentBobbingFactor = 0.0f;

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float BobbingSpeed = 10.0f;

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float CrouchBobbingMultiplier = 0.5f;

    UPROPERTY(EditAnywhere, Category = "HeadBobbing")
    float BobbingAmount = 2.5f;

    UPROPERTY(VisibleAnywhere, Category = "HeadBobbing")
    float BobbingTime; // internal timer for bobbing calculation

    UPROPERTY(VisibleAnywhere, Category = "HeadBobbing")
    FVector DefaultCameraPosition; // stores the default camera position

    UPROPERTY(EditAnywhere, Category = "Crouching")
    float StandingCapsuleHalfHeight;

    UPROPERTY(EditAnywhere, Category = "Crouching")
    float CrouchingCapsuleHalfHeight;

    UPROPERTY(EditAnywhere, Category = "Crouching")
    float MaxCrouchTransitionTime;

    // STAMINA FEATURE
    UPROPERTY(EditAnywhere, Category = "Stamina")
    float MaxStamina;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaDrainRate;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaRegenRate;

    UPROPERTY(VisibleAnywhere, Category = "Stamina")
    float CurrentStamina;

    UPROPERTY(VisibleAnywhere, Category = "Stamina")
    bool bIsExhausted = false; //prevents sprinting immediately after running out of stamina

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float ExhaustionRecoveryTime = 2.0f; //time before player can sprint again
    FTimerHandle ExhaustionTimerHandle;

    //smooth VFX transition variables
    float TargetVignetteIntensity;
    float VFXTransitionSpeed;

    // VisualFX FUNCTION DECLARATIONS
    void ApplyStaminaExhaustionEffects();
    void ApplyHeadBobbing(float DeltaTime);
    void ResetExhaustion();

    // CAMERA FUNCTION DECLARATIONS
    void Horizon_Move(float value);
    void Vertic_Move(float value);
    void Horizon_Rot(float value);
    void Vertic_Rot(float value);

    // MOVEMENT TRANSITIONS
    float TargetMoveSpeed = 0.0f;
    float CurrentMoveSpeed = 0.0f;
    float MovementInterpSpeed = 5.0f; // smaller = smoother transition
    float GetTargetFOV() const;

    // CROUCH FUNCTION/VARIABLE DECLARATIONS
    void SmoothCrouchTransition(float DeltaTime);

    FTimerHandle CrouchTimerHandle;
    bool bIsCrouchingInProgress, bWantsToCrouch, bIsSprinting;
    float CrouchCameraBaseZ, CurrentCapsuleHeight;
    float CapsuleInterpSpeed = 4.0f; // crouch speed

    Ainteraction_System* Interaction_System;

    bool IsFullyCrouched() const
    {
        return FMath::IsNearlyEqual(CurrentCapsuleHeight, CrouchingCapsuleHalfHeight, 1.0f);
    }
};