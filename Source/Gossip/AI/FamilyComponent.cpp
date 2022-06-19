// Fill out your copyright notice in the Description page of Project Settings.


#include "FamilyComponent.h"
#include "SocialComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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

	OnNewCityHallEvent.Broadcast(ECityHallEvents::Wedding, GetGuests(Other));
	SetCurrentFiancee(Other);
	OtherFamilyComp->SetCurrentFiancee(GetOwner());
	ResetOwnersAI(Other);
}

TArray<AActor*> UFamilyComponent::GetGuests(AActor* Other)
{
	TArray<AActor*>Guests;
	USocialComponent* SocialComp = Cast<USocialComponent>(GetOwner()->GetComponentByClass(USocialComponent::StaticClass()));
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(Other->GetComponentByClass(USocialComponent::StaticClass()));
	Guests.AddUnique(GetOwner());
	Guests.AddUnique(Other);
	TArray<AActor*>Friends;
	Friends.Append(SocialComp->GetFriends());
	Friends.Append(OtherSocialComp->GetFriends());
	for (AActor* Friend : Friends)
	{
		Guests.AddUnique(Friend);
	}
	return Guests;
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
	if (CurrentFiancee)
	{
		USocialComponent* FianceeSocialComp = Cast<USocialComponent>(CurrentFiancee->GetComponentByClass(USocialComponent::StaticClass()));
		SaveValues.FianceeGuid = FianceeSocialComp->Id;
	}
	return SaveValues;
}

void UFamilyComponent::RestoreState(FSaveValues SaveData)
{
	TArray<AActor*>Actors;
	UGameplayStatics::GetAllActorsOfClass(GetOwner()->GetWorld(), ACharacter::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		USocialComponent* OtherSocialComp = Cast<USocialComponent>(Actor->GetComponentByClass(USocialComponent::StaticClass()));
		if (!OtherSocialComp) continue;
		if (OtherSocialComp->Id != SaveData.FianceeGuid) continue;
		CurrentFiancee = Actor;
	}
}

