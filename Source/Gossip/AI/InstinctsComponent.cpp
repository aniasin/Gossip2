// Fill out your copyright notice in the Description page of Project Settings.


#include "InstinctsComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Gossip/Core/GossipGameMode.h"


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

	AGossipGameMode* GM = Cast<AGossipGameMode>(UGameplayStatics::GetGameMode(GetOwner()->GetWorld()));
	if (!GM) return;
	
	FTimerHandle InstinctUpdateTimerHandle;
	float OneGameHour = GM->GameHourDurationSeconds;
	float RandomDelay = FMath::RandRange(1, 20);
	GetOwner()->GetWorldTimerManager().SetTimer(InstinctUpdateTimerHandle, this, &UInstinctsComponent::InstinctsUpdate, OneGameHour, true, RandomDelay);
}

void UInstinctsComponent::SatisfyInstinct(EAIGoal Instinct)
{
	BasicInstincts[Instinct] -= 1; 
	// TODO make it variable. IE: Each Instincts will have a coefficient.
	// TODO Play AnimMontage and wait for end.
}

void UInstinctsComponent::InstinctsUpdate()
{
	for (auto Goal : BasicInstincts)
	{
		Goal.Value += 0.1; // TODO make it variable. IE: Each Instincts will have a coefficient.
	}
	SortBasicInstinctsByPriority();
}

void UInstinctsComponent::SortBasicInstinctsByPriority()
{
	BasicInstincts.ValueSort([](const float& A, const float& B) {
		return A >= B;
		});
}

	


