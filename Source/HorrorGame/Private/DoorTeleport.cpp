// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorTeleport.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADoorTeleport::ADoorTeleport()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	RootComponent = DoorMesh;

	Tags.Add(FName("Door"));

}

// Called when the game starts or when spawned
void ADoorTeleport::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorTeleport::Interact(AActor* Interactor)
{
	if (Interactor)
	{
		FVector TeleportLocation = GetActorLocation() + (GetActorForwardVector() * TeleportDistance) + TeleportOffset;
		Interactor->SetActorLocation(TeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

