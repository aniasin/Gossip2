// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialRulesComponent.h"

#include "Gossip/AI/GS_AIController.h"
#include "Gossip/Core/GS_GameInstance.h"
#include "Gossip/Core/GossipGameMode.h"

USocialRulesComponent::USocialRulesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USocialRulesComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USocialRulesComponent::NewWeddingCandidates(TMap<AActor*, AActor*> Couple)
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
		for (AActor* Candidate : WeddingCandidates)
		{
			AGS_AIController* AIController = Cast<AGS_AIController>(Candidate->GetInstigatorController());
			AIController->ResetAI();
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

