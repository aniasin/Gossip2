// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialComponent.h"
#include "GS_AIController.h"


// Sets default values for this component's properties
USocialComponent::USocialComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USocialComponent::BeginPlay()
{
	Super::BeginPlay();

}

EAlignmentState USocialComponent::RefreshKnownOthers(AActor* Other)
{
	USocialComponent* OtherAlignmentComp = Cast<USocialComponent>(Other->GetComponentByClass(USocialComponent::StaticClass()));

	FString Name = Other->GetName();
	if (!KnownOthers.Contains(Name))
	{
		FAlignment NewAlignment;
		NewAlignment.Love = 0;
		NewAlignment.Respect = 0;
		KnownOthers.Add(Name, NewAlignment);
	}
	EAlignmentState AlignmentState = GetAlignment(KnownOthers[Name].Respect, KnownOthers[Name].Love);

// 	for (auto& KnownOther : KnownOthers)
// 	{
// 		FString OtherName = KnownOther.Key;
// 		FAlignment OtherAlignment = KnownOther.Value;
// 		EAlignmentState Alignment = GetAlignment(OtherAlignment.Respect, OtherAlignment.Love);
// 		FString AlignmentString = GetEnumValueAsString("EAlignmentState", Alignment);
// 		UE_LOG(LogTemp, Log, TEXT("%s KnownOthers: %s -- %s"), *GetOwner()->GetName(), *OtherName, *AlignmentString)
// 	}
// 	UE_LOG(LogTemp, Log, TEXT("//////////////////////"))

	return AlignmentState;
}

bool USocialComponent::InitiateInteraction(AActor* Other)
{
	if (!IsValid(Other)) return false;

	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return false;

	UpdateAlignment(Other);	
	OtherSocialComp->RespondToInteraction(GetOwner());

	return true;
}

bool USocialComponent::RespondToInteraction(AActor* Other)
{
	if (!IsValid(Other)) return false;

	UpdateAlignment(Other);

	return true;
}

void USocialComponent::EndInteraction(AActor* Other)
{
	bSocializing = false;
	CurrentAlignmentState = EAlignmentState::None;
	return;
}

void USocialComponent::UpdateAlignment(AActor* Other)
{
	if (!IsValid(Other)) return;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return;

	bSocializing = true;

	FString OtherName = Other->GetName();

	FAlignment NewAlignment;
	NewAlignment = CalculateAlignmentChange(Other);
	NewAlignment.Respect += KnownOthers[OtherName].Respect;
	NewAlignment.Love += KnownOthers[OtherName].Love;

	KnownOthers.Add(OtherName, NewAlignment);

	EAlignmentState AlignmentState = GetAlignment(KnownOthers[OtherName].Respect, KnownOthers[OtherName].Love);
	CurrentAlignmentState = AlignmentState;
}

void USocialComponent::UpdateEmotionalState(TArray<EAIGoal>HungryInstincts)
{
	if (bSocializing) return;
	TMap<EEmotionalState, float>EmotionalScores;

	float Multiplier = 0;
	for (FEmotionalUpdateTable TableIndex : EmotionalUpdateTable)
	{
		HungryInstincts.Contains(TableIndex.Instinct) ? Multiplier = 1 : Multiplier = -1;

		for (auto& EmotionalState : TableIndex.EmotionalEffect)
		{
			if (!EmotionalScores.Contains(EmotionalState.Key)) EmotionalScores.Add(EmotionalState.Key, 0);
			float NewValue = EmotionalState.Value * Multiplier + EmotionalScores[EmotionalState.Key];
			EmotionalScores.Add(EmotionalState.Key, NewValue);
		}
	}

	EmotionalScores.ValueSort([](const float& A, const float& B) {
		return A > B ;
		});

	TArray<EEmotionalState>SortedEmotionalStates;
	EmotionalScores.GenerateKeyArray(SortedEmotionalStates);

	OwnEmotionalState = SortedEmotionalStates[0];
}

EAlignmentState USocialComponent::GetAlignment(float Respect, float Love)
{
	if (Love >= 0 && Respect >= 0) return EAlignmentState::Cooperative;
	if (Love >= 0 && Respect < 0) return EAlignmentState::Masterful;
	if (Love < 0 && Respect < 0) return EAlignmentState::Imperious;
	if (Love < 0 && Respect >= 0) return EAlignmentState::Submissive;
	return EAlignmentState::None;
}

AActor* USocialComponent::FindSocialPartner()
{
	AGS_AIController* AIController = Cast<AGS_AIController>(GetOwner()->GetInstigatorController());
	if (!AIController) return nullptr;

	TArray<AActor*> CurrentlyPerceivedActors = AIController->GetCurrentlyPerceivedActors();
	for (AActor* Actor : CurrentlyPerceivedActors)
	{
		if (Actor->FindComponentByClass(USocialComponent::StaticClass())) return Actor;
	}

	return nullptr;
}

FAlignment USocialComponent::CalculateAlignmentChange(AActor* Other)
{
	FAlignment AlignmentChange;
	AlignmentChange.Respect = 0;
	AlignmentChange.Love = 0;

	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return AlignmentChange;

	EAlignmentState OwnAlignment = RefreshKnownOthers(OtherSocialComp->GetOwner());
	EAlignmentState OtherAlignment = OtherSocialComp->RefreshKnownOthers(GetOwner());

	ESocialPosition OtherSocialPosition = OtherSocialComp->SocialPosition;
	EEmotionalState OtherEmotionalState = OtherSocialComp->OwnEmotionalState;

	float RespectChange = 0;
	if (OtherSocialPosition == SocialPositionLike) RespectChange += 1;
	if (OtherSocialPosition == SocialPositionHate)	RespectChange += -1;

	for (FSocialChangeTable TableIndex : SocialChangeTable)
	{
		if (TableIndex.Alignment != OwnAlignment) break;
		RespectChange += TableIndex.OtherAlignmentEffect[OtherAlignment];
	}

	float LoveChange = 0;
	if (OtherEmotionalState == EmotionalStateLike) LoveChange += 1;
	if (OtherEmotionalState == EmotionalStateHate)	LoveChange += -1;

	for (FEmotionalChangeTable TableIndex : EmotionalChangeTable)
	{
		if (TableIndex.EmotionalState != OwnEmotionalState) break;
		LoveChange += TableIndex.OtherEmotionalStateEffect[OtherEmotionalState];
	}

	AlignmentChange.Respect = RespectChange;
	AlignmentChange.Love = LoveChange;

	return AlignmentChange;
}

