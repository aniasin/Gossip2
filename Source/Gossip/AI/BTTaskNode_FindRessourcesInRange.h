// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindRessourcesInRange.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTTaskNode_FindRessourcesInRange : public UBTTaskNode
{
	GENERATED_BODY()
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

protected:
	UPROPERTY(VisibleAnywhere)
		FBlackboardKeySelector GoalKey;
	UPROPERTY(VisibleAnywhere)
		FBlackboardKeySelector ActionKey;
	UPROPERTY(EditAnywhere)
	float DistanceToTrace = 2000;

};
