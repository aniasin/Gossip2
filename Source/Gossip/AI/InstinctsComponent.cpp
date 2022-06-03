// Fill out your copyright notice in the Description page of Project Settings.


#include "InstinctsComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Gossip/Core/GossipGameMode.h"


// Sets default values for this component's properties
UInstinctsComponent::UInstinctsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;



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
	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		if (Instinct.Goal == Goal)
		{
			Instinct.GrowCoeffient += 0.1 * Instinct.UpdateMultiplier;
			Instinct.CurrentValue -= 1 * Instinct.UpdateMultiplier;
		}
		else
		{
			Instinct.GrowCoeffient -= 0.1 * Instinct.UpdateMultiplier;
		}		
	}	
	// TODO Play AnimMontage and wait for end.
}

void UInstinctsComponent::InstinctsUpdate()
{
	TArray<EAIGoal> HungryInstincts;
	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		Instinct.CurrentValue += FMath::Abs(Instinct.GrowCoeffient) * Instinct.UpdateMultiplier;
		if (Instinct.CurrentValue >= 1) HungryInstincts.AddUnique(Instinct.Goal);
	}
	OnInstinctsUpdated.Broadcast(HungryInstincts);
	SortInstinctsByPriority();
}

void UInstinctsComponent::SortInstinctsByPriority()
{
	InstinctsInfo.Sort([](const FInstinctValues& A, const FInstinctValues& B) {
		return FMath::Abs(A.CurrentValue) * FMath::Abs(A.GrowCoeffient) >= FMath::Abs(B.CurrentValue) * FMath::Abs(B.GrowCoeffient);
		});
}

