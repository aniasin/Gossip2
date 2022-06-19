// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialRulesComponent.h"
#include "PlayerOrdersComponent.h"

#include "Gossip/Core/GS_GameInstance.h"
#include "Gossip/Core/GossipGameMode.h"
#include "Gossip/AI/FamilyComponent.h"


USocialRulesComponent::USocialRulesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USocialRulesComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Only the Pretender call that
void USocialRulesComponent::NewWeddingCandidate(AActor* Pretender, AActor* Other, ECharacterProfile PretenderGender)
{	
	if (!IsValid(Other) || !IsValid(Pretender)) return;	

	WeddingQueue.Add(Pretender, Other);

	if (!TutorialFirstTime())
	{
		// If it's the first time, we're waiting a Broadcast from MenuSocialRules
		RequestWedding();
	}	
	else
	{
		UPlayerOrdersComponent* PlayerOrdersComp = Cast<UPlayerOrdersComponent>(GetOwner()->GetComponentByClass(UPlayerOrdersComponent::StaticClass()));
		PlayerOrdersComp->PlayerMoveToTarget(Pretender);
	}
}

void USocialRulesComponent::RequestWedding()
{
	TArray<AActor*>PretendersWaiting;
	WeddingQueue.GenerateKeyArray(PretendersWaiting);

	for (AActor* Pretender : PretendersWaiting)
	{
		UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Pretender->GetComponentByClass(UFamilyComponent::StaticClass()));
		if (FamilyComp)
		{
			FamilyComp->RequestWedding(WeddingQueue[Pretender], WeddingRule);
		}
	}
	WeddingQueue.Empty();
}

bool USocialRulesComponent::TutorialFirstTime()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwner()->GetWorld()->GetAuthGameMode());
	if (!GM) return false;
	if (WeddingRule.FamilySystem == EFamilySystem::None || WeddingRule.WeddingSystem == EWeddingSystem::None)
	{
		UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetOwner()->GetWorld()->GetGameInstance());
		if (!GI) return false;
		GI->LoadSocialRulesMenu();
		return true;
	}
	return false;
}

// Called from MenuSocialRules
void USocialRulesComponent::SetNewWeddingRule(FWeddingRule Rule)
{
	WeddingRule = Rule;

	AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwner()->GetWorld()->GetAuthGameMode());
	if (!GM) return;
	GM->SetWeddingRule(WeddingRule);
}

// ISaveGameInterface
FSaveValues USocialRulesComponent::CaptureState()
{
	FSaveValues SaveValues;
	SaveValues.WeddingRules = WeddingRule;
	return SaveValues;
}

void USocialRulesComponent::RestoreState(FSaveValues SaveData)
{
	WeddingRule = SaveData.WeddingRules;

	AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwner()->GetWorld()->GetAuthGameMode());
	if (!GM) return;
	GM->SetWeddingRule(WeddingRule);
}

