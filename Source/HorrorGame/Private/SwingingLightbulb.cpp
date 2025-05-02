#include "HorrorGame/HorrorGame.h"
#include "SwingingLightbulb.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Engine/World.h"

ASwingingLightbulb::ASwingingLightbulb()
{
    PrimaryActorTick.bCanEverTick = true;

    CeilingAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("CeilingAnchor"));
    RootComponent = CeilingAnchor;

    FixtureSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FixtureSphere"));
    FixtureSphere->SetupAttachment(CeilingAnchor);
    FixtureSphere->SetSimulatePhysics(false);

    LowPolyLightBulb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LowPolyLightBulb"));
    LowPolyLightBulb->SetupAttachment(FixtureSphere);
    LowPolyLightBulb->SetSimulatePhysics(true);

    // Load and assign the sphere mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
    if (SphereMeshAsset.Succeeded())
    {
        FixtureSphere->SetStaticMesh(SphereMeshAsset.Object);
    }

    // Load and assign the lightbulb mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> LightbulbMeshAsset(TEXT("StaticMesh'/Game/Props/low_poly_lamp.low_poly_lamp'"));
    if (LightbulbMeshAsset.Succeeded())
    {
        LowPolyLightBulb->SetStaticMesh(LightbulbMeshAsset.Object);
    }

    SwingConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("SwingConstraint"));
    SwingConstraint->SetupAttachment(FixtureSphere);
    SwingConstraint->SetConstrainedComponents(FixtureSphere, NAME_None, LowPolyLightBulb, NAME_None);
    SwingConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Limited, 30.0f);
    SwingConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 10.0f);
    SwingConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f);

    SlowRotation = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("SlowRotation"));
    SlowRotation->RotationRate = FRotator(0.0f, 2.0f, 0.0f);

    FixtureSphere->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
    FixtureSphere->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

    LowPolyLightBulb->SetVisibility(true);
    FixtureSphere->SetVisibility(true);
    LowPolyLightBulb->SetHiddenInGame(false);
    FixtureSphere->SetHiddenInGame(false);
}

void ASwingingLightbulb::BeginPlay()
{
    Super::BeginPlay();

    FVector InitialTorque = FVector(500.0f, 0.0f, 0.0f);
    LowPolyLightBulb->AddTorqueInDegrees(InitialTorque, NAME_None, true);
}

