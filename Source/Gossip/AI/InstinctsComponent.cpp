// Fill out your copyright notice in the Description page of Project Settings.


#include "InstinctsComponent.h"


// Sets default values for this component's properties
UInstinctsComponent::UInstinctsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	for (EAIGoal Goal : TEnumRange<EAIGoal>())
	{
		BasicInstincts.Add(Goal, 0);
	}
}

// Called when the game starts
void UInstinctsComponent::BeginPlay()
{
	Super::BeginPlay();

	SortBasicInstinctsByPriority();

}

void UInstinctsComponent::SortBasicInstinctsByPriority()
{
	BasicInstincts.ValueSort([](const float& A, const float& B) {
		return A >= B;
		});
}

	


