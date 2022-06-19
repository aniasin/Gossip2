// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "FamilyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewCityHallEvent, ECityHallEvents, Event, TArray<AActor*>, Guests);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UFamilyComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	

	UFamilyComponent();

	ECharacterProfile CharacterGender;

	void RequestWedding(AActor* Other, FWeddingRule WeddingRule);

	void SetCurrentFiancee(AActor* Fiancee) { CurrentFiancee = Fiancee; }
	void ResetOwnersAI(AActor* Other);

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

	FOnNewCityHallEvent OnNewCityHallEvent;

protected:

	virtual void BeginPlay() override;

private:
	FTimerHandle ScheduledWeddingTimerHandle;
	FWeddingRule CurrentWeddingRules;
	TArray<AActor* > Spouses;
	AActor* CurrentFiancee;		
};
