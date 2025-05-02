#include "InteractiveAnimationBase.h"

AInteractiveAnimationBase::AInteractiveAnimationBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AInteractiveAnimationBase::BeginPlay()
{
    Super::BeginPlay();
}

void AInteractiveAnimationBase::ToggleDrawer(FName ElementName)
{

    bool& bIsOpen = ElementStates.FindOrAdd(ElementName);
    bIsOpen = !bIsOpen;
}

void AInteractiveAnimationBase::OnTimelineFinished(FName ElementName)
{
    ActiveElements.Remove(ElementName);
}
