// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "PauseManager.generated.h"

UCLASS()
class HORRORGAME_API APauseManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APauseManager();

	TSubclassOf<UUserWidget> GetPauseMenuClass() const { return PauseMenuClass; }

	UFUNCTION(BlueprintCallable, Category = "UI")
	void BPResumeGame() { ResumeGame(); }
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void BPQuitGame() { QuitGame(); }

	UFUNCTION(BlueprintCallable, Category = "UI")
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ToggleQuitOptions();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void QuitToDesktop();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowSettingsMenu();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ReturnToPauseMenu();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void SaveGame();

	void TogglePauseMenu();
	void ResumeGame();
	void QuitGame();

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuClass;

	UPROPERTY()
	UUserWidget* PauseMenuWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> SettingsMenuClass; //reference to WB_OptionMenu

	UPROPERTY()
	UUserWidget* SettingsMenuWidget;

	bool bIsPaused;
};
