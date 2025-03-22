// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenuManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UInGameMenuManager::NativeConstruct()
{
    Super::NativeConstruct();
    // Additional initialization if needed
}

void UInGameMenuManager::OnResumeClicked()
{
    // Logic to resume the game
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        PlayerController->SetPause(false);
        RemoveFromViewport();

        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = false;
    }
}

void UInGameMenuManager::OnSettingsClicked()
{
    // Logic to open settings menu
    // You can create and show the settings menu here
}

void UInGameMenuManager::OnQuitClicked()
{
    // Logic to quit the game
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (IsValid(PlayerController)) // Check if PlayerController is valid
    {
        // Log the state of the widget
        UE_LOG(LogTemp, Warning, TEXT("InGameMenuManager is valid: %s"), IsValid(this) ? TEXT("true") : TEXT("false"));

        // Check if the widget is valid before removing it
        if (IsValid(this))
        {
            RemoveFromViewport();
            // Log after removal
            UE_LOG(LogTemp, Warning, TEXT("InGameMenuManager removed from viewport."));
        }

        // Reset input mode to game only
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        PlayerController->bShowMouseCursor = false; // Hide the mouse cursor

        // Quit the game
        FGenericPlatformMisc::RequestExit(false);
    }
}
