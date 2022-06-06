// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialComponent.h"
#include "GS_AIController.h"

#include "Gossip/Data/EmotionalDataAsset.h"
#include "Gossip/Data/EmotionalUpdateDataAsset.h"
#include "Gossip/Data/SocialDataAsset.h"


// Sets default values for this component's properties
USocialComponent::USocialComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#if WITH_EDITOR
void USocialComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SocialChangeTable = SocialChangeChartDataAsset->SocialChangeTable;
	EmotionalChangeTable = EmotionalChangeChartDataAsset->EmotionalChangeTable;
	EmotionalUpdateTable = EmotionalUpdtateChartDataAsset->EmotionalUpdateTable;
}
#endif WITH_EDITOR

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

	return AlignmentState;
}

bool USocialComponent::InitiateInteraction(AActor* Other)
{
	if (!IsValid(Other)) return false;

	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return false;

	bool bsuccess = UpdateAlignment(Other);
	FString Message;
	bsuccess ? Message = "Socialization was a success for" : Message = "Socialization was a failure for";
	UE_LOG(LogTemp, Log, TEXT("%s %s"), *Message, *GetOwner()->GetName())

	OtherSocialComp->RespondToInteraction(GetOwner());

	return bsuccess;
}

bool USocialComponent::RespondToInteraction(AActor* Other)
{
	if (!IsValid(Other)) return false;

	bool bSuccess =	UpdateAlignment(Other);
	FString Message;
	bSuccess ? Message = "Socialization was a success for" : Message = "Socialization was a failure for";
	UE_LOG(LogTemp, Log, TEXT("%s %s"), *Message, *GetOwner()->GetName())

	return bSuccess;
}

void USocialComponent::EndInteraction(AActor* Other)
{
	CurrentAlignmentState = EAlignmentState::None;
	return;
}

bool USocialComponent::UpdateAlignment(AActor* Other)
{
	bool bSuccess = false;
	if (!IsValid(Other)) return bSuccess;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return bSuccess;

	FString OtherName = Other->GetName();

	FAlignment NewAlignment;

	NewAlignment = CalculateAlignmentChange(Other, bSuccess);
	NewAlignment.Respect += KnownOthers[OtherName].Respect;
	NewAlignment.Love += KnownOthers[OtherName].Love;

	KnownOthers.Add(OtherName, NewAlignment);

	EAlignmentState AlignmentState = GetAlignment(KnownOthers[OtherName].Respect, KnownOthers[OtherName].Love);
	CurrentAlignmentState = AlignmentState;

	int32 AffinityMultiplier = 0;
	bSuccess ? AffinityMultiplier = 1 : AffinityMultiplier = -1;
	KnownOthers[OtherName].Affinity += AffinityMultiplier;

	return bSuccess;
}

void USocialComponent::UpdateEmotionalState(TArray<EAIGoal>HungryInstincts)
{
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

	TArray<AActor*>KnownOthersInVincinity;
	TArray<AActor*> CurrentlyPerceivedActors = AIController->GetCurrentlyPerceivedActors();
	for (AActor* Actor : CurrentlyPerceivedActors)
	{
		if (!Actor->FindComponentByClass(USocialComponent::StaticClass())) continue;
		if (KnownOthers.Contains(Actor->GetName()) && KnownOthers[Actor->GetName()].Affinity > 0) KnownOthersInVincinity.Add(Actor);
	}

	if (KnownOthersInVincinity.Num() <= 0 && CurrentlyPerceivedActors.Num() > 0)
	{
		return CurrentlyPerceivedActors[0];
	}

	if (KnownOthersInVincinity.Num() > 0)
	{
		TMap<AActor*, int32>OthersToSort;
		for (AActor* Other : KnownOthersInVincinity)
		{
			OthersToSort.Add(Other, KnownOthers[Other->GetName()].Affinity);
		}
		OthersToSort.ValueSort([](const int32& A, const int32& B) {	return A > B; });
		TArray<AActor*>SortedKeys;
		OthersToSort.GenerateKeyArray(SortedKeys);
		return SortedKeys[0];
	}

	return nullptr;
}

FAlignment USocialComponent::CalculateAlignmentChange(AActor* Other, bool &bSuccess)
{
	bSuccess = false;
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
	bSuccess = RespectChange + LoveChange > 0;

	return AlignmentChange;
}

