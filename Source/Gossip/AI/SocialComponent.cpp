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

	return AlignmentState;
}

bool USocialComponent::InitiateInteraction(AActor* Other)
{
	if (!IsValid(Other)) return false;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return false;
	UE_LOG(LogTemp, Warning, TEXT("%s is initiating interaction with %s"), *GetOwner()->GetName(), *Other->GetName())

	EAlignmentState OwnAlignment = RefreshKnownOthers(Other);
	EAlignmentState OtherAlignment = OtherSocialComp->RefreshKnownOthers(GetOwner());

	UpdateAlignment(Other, OwnAlignment, OtherAlignment);
	OtherSocialComp->UpdateAlignment(GetOwner(), OtherAlignment, OwnAlignment);
	
	OtherSocialComp->RespondToInteraction(this);

	return true;
}

bool USocialComponent::RespondToInteraction(USocialComponent* OtherSocialComp)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is responding to %s"), *GetOwner()->GetName(), *OtherSocialComp->GetOwner()->GetName())

	return true;
}

void USocialComponent::EndInteraction(AActor* Other)
{

	return;
}

void USocialComponent::UpdateAlignment(AActor* Other, EAlignmentState OwnAlignment, EAlignmentState OtherAlignment)
{
	if (!IsValid(Other)) return;

	FString OwnName = GetOwner()->GetName();
	FString OtherName = Other->GetName();

	FAlignment NewAlignment;
	NewAlignment.Love = 0;
	NewAlignment.Respect = 0;

	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));

	ESocialPosition OtherSocialPosition = OtherSocialComp->SocialPosition;

	NewAlignment.Respect = CalculateRespectChange(OwnAlignment, OtherAlignment, OtherSocialPosition) + KnownOthers[OtherName].Respect;
	NewAlignment.Love = CalculateLoveChange(OwnEmotionalState, OtherSocialComp->GetEmotionalState()) + KnownOthers[OtherName].Love;

	KnownOthers.Add(OtherName, NewAlignment);

	EAlignmentState AlignmentState = GetAlignment(KnownOthers[OtherName].Respect, KnownOthers[OtherName].Love);
	CurrentAlignmentState = AlignmentState;

	FString AlignmentString = GetEnumValueAsString<EAlignmentState>("EAlignmentState", AlignmentState);
	UE_LOG(LogTemp, Warning, TEXT("%s - Alignment: %s"), *GetOwner()->GetName(), *AlignmentString)
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

float USocialComponent::CalculateRespectChange(EAlignmentState OwnAlignment, EAlignmentState OtherAlignment, ESocialPosition OtherSocialPosition)
{
	float RespectChange = GetRespectChange(OwnAlignment, OtherAlignment);
	RespectChange += CompareSocialPositionTaste(OtherSocialPosition);

	return RespectChange;
}



float USocialComponent::CalculateLoveChange(EEmotionalState CurrentEmotionalState, EEmotionalState OtherEmotionalState)
{
	float LoveChange = GetLoveChange(OtherEmotionalState);
	LoveChange += CompareEmotionalStateTaste(OtherEmotionalState);

	return LoveChange;
}

float USocialComponent::CompareEmotionalStateTaste(EEmotionalState OtherEmotionalState)
{
	if (OtherEmotionalState == EmotionalStateLike)
	{
		return 1;
	}
	else if (OtherEmotionalState == EmotionalStateHate)
	{
		return -1;
	}
	return 0;
}

float USocialComponent::CompareSocialPositionTaste(ESocialPosition OtherSocialPosition)
{
	if (OtherSocialPosition == SocialPositionLike)
	{
		return 1;
	}
	else if (OtherSocialPosition == SocialPositionHate)
	{
		return -1;
	}
	return 0;
}

float USocialComponent::GetRespectChange(EAlignmentState OwnAlignment, EAlignmentState OtherAlignment)
{
	switch (OwnAlignment)
	{
	case EAlignmentState::Submissive:
		switch (OtherAlignment)
		{
		case EAlignmentState::Submissive:
			return -1;
		case EAlignmentState::Cooperative:
			return -1;
		case EAlignmentState::Imperious:
			return 1;
		case EAlignmentState::Masterful:
			return 1;
		}
		break;

	case EAlignmentState::Cooperative:
		switch (OtherAlignment)
		{
		case EAlignmentState::Submissive:
			return -1;
		case EAlignmentState::Cooperative:
			return 1;
		case EAlignmentState::Imperious:
			return -1;
		case EAlignmentState::Masterful:
			return 1;
		}
		break;

	case EAlignmentState::Imperious:
		switch (OtherAlignment)
		{
		case EAlignmentState::Submissive:
			return -1;
		case EAlignmentState::Cooperative:
			return -1;
		case EAlignmentState::Imperious:
			return 1;
		case EAlignmentState::Masterful:
			return -1;
		}
		break;

	case EAlignmentState::Masterful:
		switch (OtherAlignment)
		{
		case EAlignmentState::Submissive:
			return -1;
		case EAlignmentState::Cooperative:
			return 1;
		case EAlignmentState::Imperious:
			return -1;
		case EAlignmentState::Masterful:
			return 1;
		}
		break;
	}

	return 1;
}

float USocialComponent::GetLoveChange(EEmotionalState OtherEmotionalState)
{
	switch (OwnEmotionalState)
	{
	case EEmotionalState::Happy:
		switch (OtherEmotionalState)
		{
		case EEmotionalState::Happy:
			return 1;
		case EEmotionalState::Sad:
			return -1;
		case EEmotionalState::Tragic:
			return -1;
		case EEmotionalState::Energic:
			return 1;
		case EEmotionalState::Tired:
			return -1;
		case EEmotionalState::Confident:
			return 1;
		case EEmotionalState::Affraid:
			return -1;
		case EEmotionalState::Panicked:
			return -1;
		}
		break;

	case EEmotionalState::Sad:
		switch (OtherEmotionalState)
		{
		case EEmotionalState::Happy:
			return 1;
		case EEmotionalState::Sad:
			return -1;
		case EEmotionalState::Tragic:
			return -1;
		case EEmotionalState::Energic:
			return 1;
		case EEmotionalState::Tired:
			return -1;
		case EEmotionalState::Confident:
			return 1;
		case EEmotionalState::Affraid:
			return -1;
		case EEmotionalState::Panicked:
			return -1;
		}
		break;

	case EEmotionalState::Tragic:
		return -1;

	case EEmotionalState::Energic:
		return 1;

	case EEmotionalState::Tired:
		switch (OtherEmotionalState)
		{
		case EEmotionalState::Happy:
			return 1;
		case EEmotionalState::Sad:
			return -1;
		case EEmotionalState::Tragic:
			return -1;
		case EEmotionalState::Energic:
			return 1;
		case EEmotionalState::Tired:
			return -1;
		case EEmotionalState::Confident:
			return 1;
		case EEmotionalState::Affraid:
			return -1;
		case EEmotionalState::Panicked:
			return -1;
		}
		break;

	case EEmotionalState::Confident:
		return 1;

	case EEmotionalState::Affraid:
		return -1;

	case EEmotionalState::Panicked:
		switch (OtherEmotionalState)
		{
		case EEmotionalState::Happy:
			return -1;
		case EEmotionalState::Sad:
			return -1;
		case EEmotionalState::Tragic:
			return 1;
		case EEmotionalState::Energic:
			return 1;
		case EEmotionalState::Tired:
			return -1;
		case EEmotionalState::Confident:
			return 1;
		case EEmotionalState::Affraid:
			return -1;
		case EEmotionalState::Panicked:
			return -1;
		}
		break;
	}
	return 1;
}
