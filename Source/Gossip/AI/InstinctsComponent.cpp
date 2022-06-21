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


}
#endif WITH_EDITOR

// Called when the game starts
void UInstinctsComponent::BeginPlay()
{
	Super::BeginPlay();

	InstinctsInfo = InstinctDataAsset->InstinctsInfo;

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
	// Must loop twice because we need to find the actual Instinct (CurrentInstinct).
	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		if (Instinct.Goal != Goal) continue;		
		CurrentInstinct = (EAIInstinct)Instinct.Instinct;
		FMath::Clamp(Instinct.CurrentValue -= 1, 0, 10);
		break;		
	}

	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		if (CurrentInstinct == (EAIInstinct)Instinct.Instinct)
		{
			FMath::Clamp(Instinct.GrowCoeffient += 0.1, 0, 10);
		}
		if (CurrentInstinct == EAIInstinct::Assimilation && Instinct.Instinct == EAIInstinct::Conservation
			|| CurrentInstinct == EAIInstinct::Conservation && Instinct.Instinct == EAIInstinct::Reproduction
			|| CurrentInstinct == EAIInstinct::Reproduction && Instinct.Instinct == EAIInstinct::Assimilation)
		{
			FMath::Clamp(Instinct.GrowCoeffient -= 0.1, 0, 10);
		}
	}
}

void UInstinctsComponent::InstinctsUpdate()
{
	TArray<EAIGoal> HungryInstincts;
	for (FInstinctValues& Instinct : InstinctsInfo)
	{
		FMath::Clamp(Instinct.CurrentValue += Instinct.GrowCoeffient * Instinct.UpdateMultiplier, 0, 10);
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

// ISavegameInterface override
FSaveValues UInstinctsComponent::CaptureState()
{
	FSaveValues SaveValues;
	SaveValues.InstinctsValues = InstinctsInfo;
	return SaveValues;
}

void UInstinctsComponent::RestoreState(FSaveValues SaveValues)
{
	InstinctsInfo = SaveValues.InstinctsValues;
}

