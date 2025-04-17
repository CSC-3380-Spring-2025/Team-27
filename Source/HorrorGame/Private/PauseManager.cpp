// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "HorrorGameInstance.h"
#include "Components/HorizontalBox.h"

APauseManager::APauseManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsPaused = false;

}

void APauseManager::LoadMainMenu()
{
    ResumeGame();
    UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenuMap"));
}

void APauseManager::TogglePauseMenu()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    if (!bIsPaused)
    {
        //pause game
        bIsPaused = true;

        //create and show pause widget
        if (PauseMenuClass)
        {
            PauseMenuWidget = CreateWidget<UUserWidget>(PC, PauseMenuClass);
            if (PauseMenuWidget)
            {
                // Call the SetPauseManager function on the widget
                FName FunctionName = FName("SetPauseManager");
                if (PauseMenuWidget->GetClass()->FindFunctionByName(FunctionName))
                {
                    struct
                    {
                        APauseManager* Manager;
                    } Params;
                    Params.Manager = this;
                    PauseMenuWidget->ProcessEvent(PauseMenuWidget->GetClass()->FindFunctionByName(FunctionName), &Params);
                }

                PauseMenuWidget->AddToViewport(100);
            }
        }

        //set input mode to UI
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->SetShowMouseCursor(true);

        //pause the actual game behind the widget
        UGameplayStatics::SetGamePaused(GetWorld(), true);
    }
    else
    {
        ResumeGame();
    }
}

void APauseManager::ResumeGame()
{
	if (!bIsPaused) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	//hide and destroy pause menu widget
	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}

	//set input mode to game
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(false);

	//unpause actual game
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	bIsPaused = false;
}

void APauseManager::QuitGame()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
	}
}

void APauseManager::ToggleQuitOptions()
{
    if (PauseMenuWidget)
    {
        // Cast to UHorizontalBox to access specific properties
        UHorizontalBox* QuitOptionsWidget = Cast<UHorizontalBox>(PauseMenuWidget->GetWidgetFromName(FName("QuitOptionsBox"))); // Use the correct name here
        if (QuitOptionsWidget)
        {
            ESlateVisibility CurrentVisibility = QuitOptionsWidget->GetVisibility();
            QuitOptionsWidget->SetVisibility(CurrentVisibility == ESlateVisibility::Visible ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
        }
    }
}

void APauseManager::QuitToDesktop()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit, false);
    }
}

void APauseManager::ShowSettingsMenu()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC || !SettingsMenuClass) return;

    // Prevent recreating the widget if it already exists
    if (!SettingsMenuWidget)
    {
        SettingsMenuWidget = CreateWidget<UUserWidget>(PC, SettingsMenuClass);

        if (SettingsMenuWidget)
        {
            // 1. Set PauseManagerRef (calls Blueprint's SetPauseManager)
            FName SetPauseManagerFn = FName("SetPauseManager");
            if (SettingsMenuWidget->GetClass()->FindFunctionByName(SetPauseManagerFn))
            {
                struct
                {
                    APauseManager* Manager;
                } Params;
                Params.Manager = this;

                SettingsMenuWidget->ProcessEvent(SettingsMenuWidget->GetClass()->FindFunctionByName(SetPauseManagerFn), &Params);
            }

            // 2. Set bIsInGameContext (calls Blueprint's SetInGameContext)
            FName SetInGameContextFn = FName("SetInGameContext");
            if (SettingsMenuWidget->GetClass()->FindFunctionByName(SetInGameContextFn))
            {
                struct
                {
                    bool IsInGame;
                } Params;
                Params.IsInGame = true;

                SettingsMenuWidget->ProcessEvent(SettingsMenuWidget->GetClass()->FindFunctionByName(SetInGameContextFn), &Params);
            }

            // 3. Add to viewport
            SettingsMenuWidget->AddToViewport(200);
        }
    }

    // Hide pause menu while settings is open
    if (PauseMenuWidget)
    {
        PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void APauseManager::ReturnToPauseMenu()
{
    if (SettingsMenuWidget)
    {
        SettingsMenuWidget->RemoveFromParent();
        SettingsMenuWidget = nullptr;
    }

    if (PauseMenuWidget)
    {
        PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void APauseManager::SaveGame()
{
    UHorrorGameInstance* GI = Cast<UHorrorGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (GI)
    {
        GI->SaveGameProgress();
        UE_LOG(LogTemp, Warning, TEXT("Save Game requested from Pause Menu"));      // check output log to see if the save worked
    }
}

