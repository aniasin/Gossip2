// Fill out your copyright notice in the Description page of Project Settings.


#include "InstinctsComponent.h"
#include "GS_AIController.h"

// Sets default values for this component's properties
UInstinctsComponent::UInstinctsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	for (EAIGoal Goal : TEnumRange<EAIGoal>())
	{
		BasicInstincts.Add((uint8)Goal, 0);
	}
}


// Called when the game starts
void UInstinctsComponent::BeginPlay()
{
	Super::BeginPlay();


}

