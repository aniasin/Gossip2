// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FinishReproduction.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTTask_FinishReproduction : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLeader;
};
