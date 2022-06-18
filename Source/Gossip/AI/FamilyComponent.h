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

	void RequestWedding(TArray<AActor*>Couple, FWeddingRule WeddingRule);
	void ConfirmWedding(AActor* Spouse);


	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

protected:

	virtual void BeginPlay() override;

private:
	TArray<AActor*>Spouses;
		
};
