// Fill out your copyright notice in the Description page of Project Settings.


#include "InstinctsComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Gossip/Core/GossipGameMode.h"


// Sets default values for this component's properties
UInstinctsComponent::UInstinctsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	EAIInstinct Instinct = EAIInstinct::None;
	float CurrentValue = 0;
	float GrowCoefficient = 0;

	for (EAIGoal Goal : TEnumRange<EAIGoal>())
	{
		if ((uint8)Goal % 3 == 1)
		{
			// Assimilation
			Instinct = EAIInstinct::Assimilation;
			CurrentValue = 0.8;
			GrowCoefficient = 1;
		}
		else if ((uint8)Goal % 3 == 0)
		{
			// Reproduction
			Instinct = EAIInstinct::Reproduction;
			CurrentValue = -5;
			GrowCoefficient = 0;
		}
		else if ((uint8)Goal % 3 == 2)
		{
			// Conservation
			Instinct = EAIInstinct::Conservation;
			CurrentValue = -10;
			GrowCoefficient = 0;
		}
		FInstinctValues Values;
		Values.Instinct = Instinct;
		Values.CurrentValue = CurrentValue;
		Values.GrowCoeffient = GrowCoefficient;
		Goals.Add(Goal, Values);
	}
}

// Called when the game starts
void UInstinctsComponent::BeginPlay()
{
	Super::BeginPlay();



	AGossipGameMode* GM = Cast<AGossipGameMode>(UGameplayStatics::GetGameMode(GetOwner()->GetWorld()));
	if (!GM) return;
	
	FTimerHandle InstinctUpdateTimerHandle;
	float OneGameHour = GM->GameHourDurationSeconds;
	float RandomDelay = FMath::RandRange(1, 20);
	GetOwner()->GetWorldTimerManager().SetTimer(InstinctUpdateTimerHandle, this, &UInstinctsComponent::InstinctsUpdate, OneGameHour, true, RandomDelay);
}

void UInstinctsComponent::SatisfyInstinct(EAIGoal Goal)
{
	for (auto& GoalItr : Goals)
	{
		if (GoalItr.Value.Instinct == Goals[Goal].Instinct)
		{
			GoalItr.Value.GrowCoeffient += 0.1;
		}
		else
		{
			GoalItr.Value.GrowCoeffient -= 0.1;
		}
	}	
	Goals[Goal].CurrentValue -= 1;

	// TODO Play AnimMontage and wait for end.
}

void UInstinctsComponent::InstinctsUpdate()
{
	for (auto& Instinct : Goals)
	{
		Instinct.Value.CurrentValue += 0.1 * Instinct.Value.GrowCoeffient;
	}
	SortGoalsByPriority();
}

void UInstinctsComponent::SortGoalsByPriority()
{
	Goals.ValueSort([](const FInstinctValues& A, const FInstinctValues& B) {
		return A.CurrentValue * A.GrowCoeffient >= B.CurrentValue * B.GrowCoeffient;
		});
}

