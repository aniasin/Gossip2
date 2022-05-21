// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SetAIGoal.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTService_SetAIGoal : public UBTService
{
	GENERATED_BODY()

	UBTService_SetAIGoal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
