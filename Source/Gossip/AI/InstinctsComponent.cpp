// Fill out your copyright notice in the Description page of Project Settings.


#include "InstinctsComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Gossip/Core/GossipGameMode.h"


// Sets default values for this component's properties
UInstinctsComponent::UInstinctsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	float CurrentValue = 0;
	float GrowCoefficient = 0;
	for (EAIGoal Goal : TEnumRange<EAIGoal>())
	{
		switch (Goal)
		{
		case EAIGoal::Food:
			CurrentValue = 0.8;
			GrowCoefficient = 1;
			break;
		case EAIGoal::Sleep:
			CurrentValue = -10;
			GrowCoefficient = 0;
			break;
		case EAIGoal::Sex:
			CurrentValue = -10;
			GrowCoefficient = 0;
			break;
		}
		FInstinctValues Values;
		Values.CurrentValue = CurrentValue;
		Values.GrowCoeffient = GrowCoefficient;
		EAIGoal Instinct = Goal;
		BasicInstincts.Add(Goal, Values);
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

void UInstinctsComponent::SatisfyInstinct(EAIGoal Instinct)
{
	BasicInstincts[Instinct].CurrentValue -= 1;
	BasicInstincts[Instinct].GrowCoeffient += 0.1;

	// TODO Play AnimMontage and wait for end.
}

void UInstinctsComponent::InstinctsUpdate()
{
	for (auto& Instinct : BasicInstincts)
	{
		Instinct.Value.CurrentValue += 0.1 * Instinct.Value.GrowCoeffient;
	}
	SortBasicInstinctsByPriority();
}

void UInstinctsComponent::SortBasicInstinctsByPriority()
{
	BasicInstincts.ValueSort([](const FInstinctValues& A, const FInstinctValues& B) {
		return A.CurrentValue * A.GrowCoeffient >= B.CurrentValue * B.GrowCoeffient;
		});
}

