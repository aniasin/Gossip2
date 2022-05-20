// Fill out your copyright notice in the Description page of Project Settings.


#include "Gossip/AI/AlignmentComponent.h"
#include "GS_AIController.h"

// Sets default values for this component's properties
UAlignmentComponent::UAlignmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAlignmentComponent::BeginPlay()
{
	Super::BeginPlay();		;
	GetWorld()->GetTimerManager().SetTimer(SetGoalTimerHandle, this, &UAlignmentComponent::SetCurrentGoal, 1, true);
}

void UAlignmentComponent::SetCurrentGoal()
{
	if (!AIController) return;
	bool bRun = false;
	uint8 PreviousGoal = AIController->GetAIGoal();
	uint8 NewGoal = (uint8)EAIGoal::None;
	if (BasicInstincts.Feed > 0.8)
	{
		NewGoal = (uint8)EAIGoal::Food;
		bRun = true;
	}
	if (BasicInstincts.Sleep > 0.8)
	{
		NewGoal = (uint8)EAIGoal::Sleep;
	}
	if (BasicInstincts.Sex > 0.8)
	{
		NewGoal = (uint8)EAIGoal::Sex;
	}
	if (NewGoal != PreviousGoal)
	{
		AIController->SetAIGoal(NewGoal);
		OnAIGoalChanged.Broadcast(bRun);
	}
}

void UAlignmentComponent::NewActorInVincinity(AActor* Other)
{
	UAlignmentComponent* OtherAlignmentComp = Cast<UAlignmentComponent>(Other->GetComponentByClass(UAlignmentComponent::StaticClass()));

	FString Name = Other->GetName();
	if (!KnownOther.Contains(Name))
	{
		FAlignment NewAlignment;
		NewAlignment.Love = Alignment_TEST.Love;
		NewAlignment.Respect = Alignment_TEST.Respect;
		KnownOther.Add(Name, NewAlignment);
	}
	EAlignmentState AlignmentState = GetAlignment(KnownOther[Name].Respect, KnownOther[Name].Love);

	FString AlignmentString = GetEnumValueAsString<EAlignmentState>("EAlignmentState", AlignmentState);
	UE_LOG(LogTemp, Warning, TEXT("Alignment: %s"), *AlignmentString)
}

EAlignmentState UAlignmentComponent::GetAlignment(float Respect, float Love)
{
	if (Love > 0 && Respect > 0) return EAlignmentState::Cooperative;
	if (Love > 0 && Respect < 0) return EAlignmentState::Masterful;
	if (Love < 0 && Respect < 0) return EAlignmentState::Imperious;
	if (Love < 0 && Respect > 0) return EAlignmentState::Submissive;
	return EAlignmentState::None;
}




