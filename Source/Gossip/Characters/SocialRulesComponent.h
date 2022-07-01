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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeddingRule WeddingRule;

	void SetNewWeddingRule(FWeddingRule Rule);

	void NewWeddingCandidate(AActor* Pretender, AActor* Other, ECharacterProfile PretenderGender);

	UFUNCTION()
	void RequestWedding();

	bool TutorialFirstTime();

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

protected:

	virtual void BeginPlay() override;

private:
	TMap<AActor*, AActor*> WeddingQueue;
};
