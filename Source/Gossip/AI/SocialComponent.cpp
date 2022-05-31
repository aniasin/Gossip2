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

EAlignmentState USocialComponent::CalculateAlignmentWithOther(AActor* Other)
{
	USocialComponent* OtherAlignmentComp = Cast<USocialComponent>(Other->GetComponentByClass(USocialComponent::StaticClass()));

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

	return AlignmentState;
}

bool USocialComponent::InitiateInteraction(AActor* Other)
{
	if (!IsValid(Other)) return false;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (!IsValid(OtherSocialComp)) return false;

	EAlignmentState Alignment = CalculateAlignmentWithOther(Other);

	UE_LOG(LogTemp, Warning, TEXT("%s is initiating interaction with %s"), *GetOwner()->GetName(), *Other->GetName())
	OtherSocialComp->RespondToInteraction(this);

	return true;
}

bool USocialComponent::RespondToInteraction(USocialComponent* OtherSocialComp)
{
	UE_LOG(LogTemp, Warning, TEXT("%s is responding to %s"), *GetOwner()->GetName(), *OtherSocialComp->GetOwner()->GetName())
	EAlignmentState OtherAlignment = OtherSocialComp->CalculateAlignmentWithOther(this->GetOwner());

	return true;
}

void USocialComponent::EndInteraction(AActor* Other)
{
	FAlignment NewAlignment;
	NewAlignment.Love = 0;
	NewAlignment.Respect = 0;
	FString OtherName = Other->GetName();

	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->FindComponentByClass(USocialComponent::StaticClass()));
	if (SocialPositionLike == OtherSocialComp->SocialPosition)
	{
		NewAlignment.Love += 0.1;
		NewAlignment.Respect += 0.1;
		KnownOther[OtherName] = NewAlignment;
	}
	else if (SocialPositionHate == OtherSocialComp->SocialPosition)
	{
		NewAlignment.Love -= 0.1;
		NewAlignment.Respect -= 0.1;
		KnownOther[OtherName] = NewAlignment;
	}
}

EAlignmentState USocialComponent::GetAlignment(float Respect, float Love)
{
	if (Love > 0 && Respect > 0) return EAlignmentState::Cooperative;
	if (Love > 0 && Respect < 0) return EAlignmentState::Masterful;
	if (Love < 0 && Respect < 0) return EAlignmentState::Imperious;
	if (Love < 0 && Respect > 0) return EAlignmentState::Submissive;
	return EAlignmentState::None;
}

AActor* USocialComponent::FindSocialPartner()
{
	AGS_AIController* AIController = Cast<AGS_AIController>(GetOwner()->GetInstigatorController());
	if (!AIController) return false;

	TArray<AActor*> CurrentlyPerceivedActors = AIController->GetCurrentlyPerceivedActors();
	for (AActor* Actor : CurrentlyPerceivedActors)
	{
		if (Actor->FindComponentByClass(USocialComponent::StaticClass())) return Actor;
	}

	return nullptr;
}

