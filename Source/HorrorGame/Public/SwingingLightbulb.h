#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwingingLightbulb.generated.h"

UCLASS()
class HORRORGAME_API ASwingingLightbulb : public AActor
{
    GENERATED_BODY()

public:
    ASwingingLightbulb();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    USceneComponent* CeilingAnchor;

    UPROPERTY()
    UStaticMeshComponent* FixtureSphere;

    UPROPERTY()
    UStaticMeshComponent* LowPolyLightBulb;

    UPROPERTY()
    UPhysicsConstraintComponent* SwingConstraint;

    UPROPERTY()
    URotatingMovementComponent* SlowRotation;
};