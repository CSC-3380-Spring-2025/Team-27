// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuManager.generated.h"

/**
 * 
 */
UCLASS()
class HORRORGAME_API UInGameMenuManager : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnResumeClicked();

	UFUNCTION(BlueprintCallable)
	void OnSettingsClicked();

	UFUNCTION(BlueprintCallable)
	void OnQuitClicked();

	
};
