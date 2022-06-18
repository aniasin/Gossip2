// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialRulesComponent.h"

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

// Only the Pretender (Couple.Key) call that
void USocialRulesComponent::NewWeddingCandidates(TMap<AActor*, AActor*> Couple, ECharacterProfile PretenderGender)
{
	TArray<AActor*>WeddingCandidates;
	Couple.GetKeys(WeddingCandidates);
	WeddingCandidates.Add(Couple[WeddingCandidates[0]]);
	if (WeddingCandidates.Num() == 2)
	{

		AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwner()->GetWorld()->GetAuthGameMode());
		if (!GM) return;
		if (!GM->GetWeddingSeenOnce())
		{
			UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetOwner()->GetWorld()->GetGameInstance());
			if (!GI) return;
			GI->OpenSocialRuleMenu();
			GM->SetWeddingSeenOnce();
		}	
		UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(WeddingCandidates[0]->GetComponentByClass(UFamilyComponent::StaticClass()));
		if (FamilyComp)
		{
			FamilyComp->RequestWedding(WeddingCandidates, WeddingRule);
		}
	}
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
	if (WeddingRule.FamilySystem != EFamilySystem::None || WeddingRule.WeddingSystem == EWeddingSystem::None)
	{
		GM->SetWeddingSeenOnce();
	}
}

