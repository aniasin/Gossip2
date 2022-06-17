// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "SocialRulesComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API USocialRulesComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	
	USocialRulesComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeddingRule WeddingRule;

	void SetNewWeddingRule(FWeddingRule Rule);

	void NewWeddingCandidates(TMap<AActor*, AActor*> Candidates);

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

protected:

	virtual void BeginPlay() override;

private:
	
		
};
