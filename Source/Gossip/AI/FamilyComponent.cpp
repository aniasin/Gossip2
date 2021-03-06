// Fill out your copyright notice in the Description page of Project Settings.


#include "FamilyComponent.h"
#include "SocialComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "Gossip/AI/GS_AIController.h" 
#include "Gossip/Core/GossipGameMode.h"

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
	OtherFamilyComp->CurrentWeddingRules = WeddingRule;
	if (WeddingRule.WeddingSystem == EWeddingSystem::Monogamy && Spouses.Num() > 0 
		|| IsValid(CurrentFiancee) || Spouses.Num() >= 3 || Spouses.Contains(Other)) 
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

void UFamilyComponent::CityHallCalling(FVector Location)
{
	AGS_AIController* AIController = Cast<AGS_AIController>(GetOwner()->GetInstigatorController());
	AIController->RequestMoveToLocation(Location, EAIStatus::CityHallCall);
}

void UFamilyComponent::Marry()
{
	if (!CurrentFiancee) return;
	Spouses.AddUnique(CurrentFiancee);

	EFamilySystem FamilySystem = CurrentWeddingRules.FamilySystem;
	switch (FamilySystem)
	{
	case EFamilySystem::Matriarcal:
		if (CharacterGender == ECharacterProfile::Female) break;
		ChangeName(CurrentFiancee);
		OnMovingShelter.Broadcast(CurrentFiancee);
		break;
	case EFamilySystem::Patriarcal:
		if (CharacterGender == ECharacterProfile::Male) break;
		ChangeName(CurrentFiancee);
		OnMovingShelter.Broadcast(CurrentFiancee);
		break;
	case EFamilySystem::Free:
		break;
	}
	CurrentFiancee = nullptr;
}

void UFamilyComponent::ChangeName(AActor* Actor)
{
	UFamilyComponent* OtherFamilyComp = Cast<UFamilyComponent>(Actor->GetComponentByClass(UFamilyComponent::StaticClass()));
	CharacterName.LastName = OtherFamilyComp->CharacterName.LastName;
	OnLastNameChanged.Broadcast(CharacterName);
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

	SaveValues.CharacterName = CharacterName;

	for (AActor* Spouse : Spouses)
	{
		UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Spouse->GetComponentByClass(UFamilyComponent::StaticClass()));
		SaveValues.Guids.AddUnique(FamilyComp->Id);
	}
	

	return SaveValues;
}

void UFamilyComponent::RestoreState(FSaveValues SaveData)
{
	CharacterName = SaveData.CharacterName;

	TArray<AActor*>Actors;
	UGameplayStatics::GetAllActorsOfClass(GetOwner()->GetWorld(), ACharacter::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		USocialComponent* OtherSocialComp = Cast<USocialComponent>(Actor->GetComponentByClass(USocialComponent::StaticClass()));
		if (!OtherSocialComp) continue;
		if (OtherSocialComp->Id != SaveData.FianceeGuid && !SaveData.Guids.Contains(OtherSocialComp->Id)) continue;
		if (OtherSocialComp->Id == SaveData.FianceeGuid)
		{
			CurrentFiancee = Actor;
			continue;
		}
		if (SaveData.Guids.Contains(OtherSocialComp->Id))
		{
			Spouses.Add(Actor);
			continue;
		}
	}
}

