// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialComponent.h"
#include "GS_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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


}
#endif WITH_EDITOR

void USocialComponent::BeginPlay()
{
	Super::BeginPlay();

	SocialChangeTable = SocialChangeChartDataAsset->SocialChangeTable;
	EmotionalChangeTable = EmotionalChangeChartDataAsset->EmotionalChangeTable;
	EmotionalUpdateTable = EmotionalUpdtateChartDataAsset->EmotionalUpdateTable;
}

EAlignmentState USocialComponent::RefreshKnownOthers(AActor* Other)
{
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->GetComponentByClass(USocialComponent::StaticClass()));

	FGuid Guid = OtherSocialComp->Id;
	if (!KnownOthers.Contains(Guid))
	{
		FAlignment NewAlignment;
		NewAlignment.Love = 0;
		NewAlignment.Respect = 0;
		NewAlignment.Proximity = 0;
		NewAlignment.Gender = OtherSocialComp->CharacterProfile;
		KnownOthers.Add(Guid, NewAlignment);	
	}
	EAlignmentState AlignmentState = GetAlignment(KnownOthers[Guid].Respect, KnownOthers[Guid].Love);

	return AlignmentState;
}

int32 USocialComponent::InitiateInteraction(AActor* Other)
{
	if (!IsValid(Other)) return 0;

	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return 0;

	UpdateAlignment(Other);
	OtherSocialComp->RespondToInteraction(GetOwner());

	UpdateFriendList(GetOwner(), Other, KnownOthers[OtherSocialComp->Id].Proximity);
	return KnownOthers[OtherSocialComp->Id].Proximity;
}

int32 USocialComponent::RespondToInteraction(AActor* Other)
{
	if (!IsValid(Other)) return 0;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));

	UpdateAlignment(Other);

	UpdateFriendList(GetOwner(), Other, KnownOthers[OtherSocialComp->Id].Proximity);
	return KnownOthers[OtherSocialComp->Id].Proximity;
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

	FGuid OtherGuid = OtherSocialComp->Id;

	FAlignment NewAlignment;
	NewAlignment = CalculateAlignmentChange(Other);
	NewAlignment.Respect += KnownOthers[OtherGuid].Respect;
	NewAlignment.Love += KnownOthers[OtherGuid].Love;
	int32 NewProximity = FMath::Clamp(NewAlignment.Proximity += KnownOthers[OtherGuid].Proximity, 0, 10);
	KnownOthers[OtherGuid].Proximity = NewProximity;
	KnownOthers.Add(OtherGuid, NewAlignment);

	K2_Dialog(NewAlignment.Proximity);

	EAlignmentState AlignmentState = GetAlignment(KnownOthers[OtherGuid].Respect, KnownOthers[OtherGuid].Love);
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
		USocialComponent* OtherSocialComp = Cast<USocialComponent>(Actor->GetComponentByClass(USocialComponent::StaticClass()));
		if (!OtherSocialComp) continue;
		if (!KnownOthers.Contains(OtherSocialComp->Id) || KnownOthers[OtherSocialComp->Id].Proximity <= -2) continue;
		AGS_AIController* OtherController = Cast<AGS_AIController>(Actor->GetInstigatorController());

		EAIStatus AIStatus = (EAIStatus)OtherController->BlackboardComponent->GetValueAsEnum("AIStatus");
		EAIGoal AIGoal = (EAIGoal)OtherController->BlackboardComponent->GetValueAsEnum("Goal");
		if (AIStatus == EAIStatus::Socialize || AIGoal == EAIGoal::Sleep || AIStatus == EAIStatus::Follow || AIStatus == EAIStatus::LeadHome) continue;
		KnownOthersInVincinity.Add(Actor);
	}

	if (KnownOthersInVincinity.Num() <= 0 && CurrentlyPerceivedActors.Num() > 0)
	{
		for (AActor* Actor : CurrentlyPerceivedActors)
		{
			if (!Actor->FindComponentByClass(USocialComponent::StaticClass())) continue;
			AGS_AIController* OtherController = Cast<AGS_AIController>(Actor->GetInstigatorController());
			EAIStatus AIStatus = (EAIStatus)OtherController->BlackboardComponent->GetValueAsEnum("AIStatus");
			EAIGoal AIGoal = (EAIGoal)OtherController->BlackboardComponent->GetValueAsEnum("Goal");
			if (AIStatus == EAIStatus::Socialize || AIGoal == EAIGoal::Sleep || AIStatus == EAIStatus::Follow || AIStatus == EAIStatus::LeadHome) continue;

			return Actor;
		}
	}

	if (KnownOthersInVincinity.Num() > 0)
	{
		TMap<AActor*, int32>OthersToSort;
		for (AActor* Other : KnownOthersInVincinity)
		{
			USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->GetComponentByClass(USocialComponent::StaticClass()));
			if (!OtherSocialComp) continue;
			OthersToSort.Add(Other, KnownOthers[OtherSocialComp->Id].Proximity);
		}
		OthersToSort.ValueSort([](const int32& A, const int32& B) {	return A > B; });
		TArray<AActor*>SortedKeys;
		OthersToSort.GenerateKeyArray(SortedKeys);

		return SortedKeys[0];
	}
	return nullptr;
}

FVector USocialComponent::FindFriendShelter()
{
	if (GetFriends().IsEmpty()) return FVector::ZeroVector;
	const TArray<AActor*> FriendsArray = GetFriends();
	AActor* Friend = FriendsArray[FMath::RandRange(0, FriendsArray.Num() - 1)];
	AGS_AIController* FriendController = Cast<AGS_AIController>(Friend->GetInstigatorController());
	if (!FriendController) return FVector::ZeroVector;
	return FriendController->BlackboardComponent->GetValueAsVector("HomeLocation");
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
	if (OtherSocialPosition == SocialPositionLike) RespectChange += .5;
	if (OtherSocialPosition == SocialPositionHate)	RespectChange += -.5;

	for (FSocialChangeTable TableIndex : SocialChangeTable)
	{
		if (TableIndex.Alignment != OwnAlignment) continue;
		RespectChange += TableIndex.OtherAlignmentEffect[OtherAlignment];
	}

	float LoveChange = 0;
	if (OtherEmotionalState == EmotionalStateLike) LoveChange += .5;
	if (OtherEmotionalState == EmotionalStateHate)	LoveChange += -.5;

	for (FEmotionalChangeTable TableIndex : EmotionalChangeTable)
	{
		if (TableIndex.EmotionalState != OwnEmotionalState) continue;
		LoveChange += TableIndex.OtherEmotionalStateEffect[OtherEmotionalState];
	}

	AlignmentChange.Respect = RespectChange;
	AlignmentChange.Love = LoveChange;
	int32 ProximityChange = 0;
	RespectChange + LoveChange >= 0 ? ProximityChange = 1 : ProximityChange = -1;
	AlignmentChange.Proximity = ProximityChange;

	return AlignmentChange;
}

void USocialComponent::UpdateFriendList(AActor* Actor, AActor* Other, int32 Proximity)
{
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->GetComponentByClass(USocialComponent::StaticClass()));
	if (Proximity >= ProximityScoreForFiancee / 2)
	{
		FriendsGuid.AddUnique(OtherSocialComp->Id);
		Friends.AddUnique(Other);
		return;
	}
	if (Friends.Contains(Other) && FriendsGuid.Contains(OtherSocialComp->Id))
	{
		FriendsGuid.Remove(OtherSocialComp->Id);
		Friends.Remove(Other);
	}
}

// ISavegameInterface override
FSaveValues USocialComponent::CaptureState()
{
	FSaveValues SaveValues;
	SaveValues.KnownOthers = KnownOthers;
	return SaveValues;
}

void USocialComponent::RestoreState(FSaveValues SaveValues)
{
	KnownOthers = SaveValues.KnownOthers;

	TArray<AActor*> NPCs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), NPCs);

	for (auto& KnownOther : KnownOthers)
	{
		for (AActor* NPC : NPCs)
		{
			USocialComponent* OtherSocialComp = Cast<USocialComponent>(NPC->GetComponentByClass(USocialComponent::StaticClass()));
			if (!OtherSocialComp) continue;
			if (KnownOther.Key != OtherSocialComp->Id) continue;
			if (KnownOther.Value.Proximity < 0) continue;
			Friends.Add(NPC);
			FriendsGuid.Add(KnownOther.Key);
		}
	}
}

