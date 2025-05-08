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
    void OnTimelineFinished(FName ElementName);

protected:
    virtual void BeginPlay() override;

    // maps open/closed state per element
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TMap<FName, bool> ElementStates;

    // this prevents spamming Timeline
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
    TSet<FName> ActiveElements;
};