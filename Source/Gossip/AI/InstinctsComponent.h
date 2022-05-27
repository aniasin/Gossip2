// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "Components/ActorComponent.h"

#include "InstinctsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInstinctsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstinctsComponent();

	// Base Value for each
	UPROPERTY(EditAnywhere, Category = "Instincts")
	TMap<EAIGoal, FInstinctValues> Goals;

	void SatisfyInstinct(EAIGoal Goal);


protected:
	virtual void BeginPlay() override;

private:
	void InstinctsUpdate();
	void SortGoalsByPriority();
		
};
