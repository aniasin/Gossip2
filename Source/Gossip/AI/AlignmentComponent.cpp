// Fill out your copyright notice in the Description page of Project Settings.


#include "Gossip/AI/AlignmentComponent.h"
#include "Gossip/Items/InventoryComponent.h"
#include "GS_AIController.h"

// Sets default values for this component's properties
UAlignmentComponent::UAlignmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAlignmentComponent::BeginPlay()
{
	Super::BeginPlay();
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




