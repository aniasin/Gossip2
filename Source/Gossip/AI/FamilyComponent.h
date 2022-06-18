// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "FamilyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UFamilyComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	

	UFamilyComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GameHoursCoolDownBetweenWeddings = 12;

	ECharacterProfile CharacterGender;

	void RequestWedding(AActor* Other, FWeddingRule WeddingRule);
	void ScheduleWedding(AActor* Fiancee);
	void StartWedding();

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

protected:

	virtual void BeginPlay() override;

private:
	FTimerHandle ScheduledWeddingTimerHandle;
	FWeddingRule CurrentWeddingRules;
	TArray<AActor* > Spouses;
	AActor* CurrentFiancee;
		
};
