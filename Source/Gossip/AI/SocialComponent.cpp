// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialComponent.h"
#include "GS_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->GetComponentByClass(USocialComponent::StaticClass()));

	FString Name = Other->GetName();
	if (!KnownOthers.Contains(Name))
	{
		FAlignment NewAlignment;
		NewAlignment.Love = 0;
		NewAlignment.Respect = 0;
		NewAlignment.Proximity = 0;
		NewAlignment.Gender = OtherSocialComp->CharacterProfile;
		KnownOthers.Add(Name, NewAlignment);
	}
	EAlignmentState AlignmentState = GetAlignment(KnownOthers[Name].Respect, KnownOthers[Name].Love);

	return AlignmentState;
}

int32 USocialComponent::InitiateInteraction(AActor* Other)
{
	if (!IsValid(Other)) return 0;

	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return 0;

	UpdateAlignment(Other);
	FString Message;
	KnownOthers[Other->GetName()].Proximity > 0 ? Message = "Socialization was a success for" : Message = "Socialization was a failure for";
	UE_LOG(LogTemp, Log, TEXT("%s %s Proximity: %i"), *Message, *GetOwner()->GetName(), KnownOthers[Other->GetName()].Proximity)

	OtherSocialComp->RespondToInteraction(GetOwner());

	return KnownOthers[Other->GetName()].Proximity;
}

int32 USocialComponent::RespondToInteraction(AActor* Other)
{
	if (!IsValid(Other)) return 0;

	UpdateAlignment(Other);
	FString Message;
	KnownOthers[Other->GetName()].Proximity > 0 ? Message = "Socialization was a success for" : Message = "Socialization was a failure for";
	UE_LOG(LogTemp, Log, TEXT("%s %s Proximity: %i"), *Message, *GetOwner()->GetName(), KnownOthers[Other->GetName()].Proximity)

	return KnownOthers[Other->GetName()].Proximity;
}

void USocialComponent::EndInteraction(AActor* Other)
{
	CurrentAlignmentState = EAlignmentState::None;
}

void USocialComponent::UpdateAlignment(AActor* Other)
{
	if (!IsValid(Other)) return;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return;

	FString OtherName = Other->GetName();

	FAlignment NewAlignment;
	NewAlignment = CalculateAlignmentChange(Other);
	NewAlignment.Respect += KnownOthers[OtherName].Respect;
	NewAlignment.Love += KnownOthers[OtherName].Love;
	int32 ProximityChanged = NewAlignment.Proximity + KnownOthers[OtherName].Proximity;
	NewAlignment.Proximity = FMath::Clamp(ProximityChanged, -10, 10);
	KnownOthers.Add(OtherName, NewAlignment);

	EAlignmentState AlignmentState = GetAlignment(KnownOthers[OtherName].Respect, KnownOthers[OtherName].Love);
	CurrentAlignmentState = AlignmentState;

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
		if (!KnownOthers.Contains(Actor->GetName()) || KnownOthers[Actor->GetName()].Proximity <= 0) continue;
		if (AIController->BlackboardComponent->GetValueAsEnum("AIStatus") == (uint8)EAIStatus::Socialize) continue;

		KnownOthersInVincinity.Add(Actor);
	}

	if (KnownOthersInVincinity.Num() <= 0 && CurrentlyPerceivedActors.Num() > 0)
	{
		for (AActor* Actor : CurrentlyPerceivedActors)
		{
			if (!Actor->FindComponentByClass(USocialComponent::StaticClass())) continue;
			if (AIController->BlackboardComponent->GetValueAsEnum("AIStatus") == (uint8)EAIStatus::Socialize) continue;
			return Actor;
		}
	}

	if (KnownOthersInVincinity.Num() > 0)
	{
		TMap<AActor*, int32>OthersToSort;
		for (AActor* Other : KnownOthersInVincinity)
		{
			OthersToSort.Add(Other, KnownOthers[Other->GetName()].Proximity);
		}
		OthersToSort.ValueSort([](const int32& A, const int32& B) {	return A > B; });
		TArray<AActor*>SortedKeys;
		OthersToSort.GenerateKeyArray(SortedKeys);
		return SortedKeys[0];
	}
	return nullptr;
}

FAlignment USocialComponent::CalculateAlignmentChange(AActor* Other)
{
	FAlignment AlignmentChange;
	AlignmentChange.Respect = 0;
	AlignmentChange.Love = 0;
	AlignmentChange.Proximity = 0;
	AlignmentChange.Gender = CharacterProfile;

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

	AlignmentChange.Respect = RespectChange + KnownOthers[Other->GetName()].Proximity / 10;
	AlignmentChange.Love = LoveChange + KnownOthers[Other->GetName()].Proximity / 10;
	AlignmentChange.Proximity = RespectChange + LoveChange > 0 ? 1 : -1;

	return AlignmentChange;
}

