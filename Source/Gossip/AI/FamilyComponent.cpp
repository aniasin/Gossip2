// Fill out your copyright notice in the Description page of Project Settings.


#include "FamilyComponent.h"
#include "SocialComponent.h"

#include "Gossip/AI/GS_AIController.h" 

UFamilyComponent::UFamilyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UFamilyComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Only the Pretender will call that.
void UFamilyComponent::RequestWedding(AActor* Other, FWeddingRule WeddingRule)
{
	UFamilyComponent* OtherFamilyComp = Cast<UFamilyComponent>(Other->GetComponentByClass(UFamilyComponent::StaticClass()));

	CurrentWeddingRules = WeddingRule;
	if (WeddingRule.WeddingSystem == EWeddingSystem::Monogamy && Spouses.Num() > 0 || IsValid(CurrentFiancee)) 
	{ 
	ResetOwnersAI(Other);
	return; 
	}

	TArray<AActor*>Guests;
	Guests.AddUnique(GetOwner());
	Guests.AddUnique(Other);
	OnNewCityHallEvent.Broadcast(ECityHallEvents::Wedding, Guests);
	SetCurrentFiancee(Other);
	OtherFamilyComp->SetCurrentFiancee(GetOwner());
	ResetOwnersAI(Other);
}

void UFamilyComponent::ResetOwnersAI(AActor* Other)
{
	AGS_AIController* AIController = Cast<AGS_AIController>(GetOwner()->GetInstigatorController());
	AGS_AIController* OtherAIController = Cast<AGS_AIController>(Other->GetInstigatorController());
	if (!AIController || !OtherAIController) return;
	AIController->ResetAI();
	OtherAIController->ResetAI();
}

// ISaveGameInterface
FSaveValues UFamilyComponent::CaptureState()
{
	FSaveValues SaveValues;

	return SaveValues;
}

void UFamilyComponent::RestoreState(FSaveValues SaveData)
{

}

