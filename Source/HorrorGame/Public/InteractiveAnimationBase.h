#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveAnimationBase.generated.h"

UCLASS()
class HORRORGAME_API AInteractiveAnimationBase : public AActor
{
    GENERATED_BODY()

public:
    AInteractiveAnimationBase();

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void ToggleDrawer(FName ElementName);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void OnTimelineFinished(FName ElementName); // called at Timeline end

protected:
    virtual void BeginPlay() override;

    // Maps open/closed state per element
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TMap<FName, bool> ElementStates;

    // Prevents spamming Timeline
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TSet<FName> ActiveElements;
};