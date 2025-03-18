// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorTeleport.generated.h"

UCLASS()
class HORRORGAME_API ADoorTeleport : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorTeleport();

	UFUNCTION()
	void Interact(AActor* Interactor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportDistance = 200.0f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, Category = "Teleport")
	FVector TeleportOffset;

};
