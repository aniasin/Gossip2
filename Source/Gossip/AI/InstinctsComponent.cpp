// Fill out your copyright notice in the Description page of Project Settings.


#include "InstinctsComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Gossip/Data/InstinctsDataAsset.h"
#include "Gossip/Core/GossipGameMode.h"


// Sets default values for this component's properties
UInstinctsComponent::UInstinctsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;	

}

#if WITH_EDITOR
void UInstinctsComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	InstinctsInfo = InstinctDataAsset->InstinctsInfo;
}
#endif WITH_EDITOR

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
	EAIInstinct CurrentInstinct = EAIInstinct::None;
	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		if (Instinct.Goal == Goal)
		{
			CurrentInstinct = (EAIInstinct)Instinct.Instinct;
			Instinct.GrowCoeffient += 0.1 * Instinct.UpdateMultiplier;
			float NewValue = FMath::Abs(Instinct.CurrentValue);
			NewValue -= 1 * Instinct.UpdateMultiplier;
			Instinct.CurrentValue = NewValue;
			break;
		}
	}
	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		if (Instinct.Goal == Goal) continue;
		if (CurrentInstinct == EAIInstinct::Assimilation && Instinct.Instinct == EAIInstinct::Conservation
			|| CurrentInstinct == EAIInstinct::Conservation && Instinct.Instinct == EAIInstinct::Reproduction
			|| CurrentInstinct == EAIInstinct::Reproduction && Instinct.Instinct == EAIInstinct::Assimilation)
		{
			Instinct.GrowCoeffient -= 0.1 * Instinct.UpdateMultiplier; 
		}
	}	
}

void UInstinctsComponent::InstinctsUpdate()
{
	TArray<EAIGoal> HungryInstincts;
	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		Instinct.CurrentValue += Instinct.GrowCoeffient * Instinct.UpdateMultiplier;
		if (FMath::Abs(Instinct.CurrentValue) >= 1) HungryInstincts.AddUnique(Instinct.Goal);
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

