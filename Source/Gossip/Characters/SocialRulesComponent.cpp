// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialRulesComponent.h"

USocialRulesComponent::USocialRulesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USocialRulesComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USocialRulesComponent::NewWeddingCandidate(AActor* Candidate)
{
	WeddingCandidates.AddUnique(Candidate);
	if (WeddingCandidates.Num() == 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s and %s are ready to marry!"), *WeddingCandidates[0]->GetName(), *WeddingCandidates[1]->GetName())
	}
}

// ISaveGameInterface
FSaveValues USocialRulesComponent::CaptureState()
{
	FSaveValues SaveValues;

	return SaveValues;
}

void USocialRulesComponent::RestoreState(FSaveValues SaveData)
{

}

