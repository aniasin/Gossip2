// Fill out your copyright notice in the Description page of Project Settings.


#include "SocialRulesComponent.h"

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

void USocialRulesComponent::NewWeddingCandidate(AActor* Candidate)
{
	WeddingCandidates.AddUnique(Candidate);
	if (WeddingCandidates.Num() == 2)
	{

		AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwner()->GetWorld()->GetAuthGameMode());
		if (!GM) return;
		if (!GM->GetWeddingSeenOnce())
		{
			UE_LOG(LogTemp, Warning, TEXT("%s and %s are ready to marry!"), *WeddingCandidates[0]->GetName(), *WeddingCandidates[1]->GetName())
				UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetOwner()->GetWorld()->GetGameInstance());
			if (!GI) return;
			GI->LoadSocialRulesMenu();
			GM->SetWeddingSeenOnce();
		}
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

