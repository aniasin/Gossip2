// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Gossip/Data/DataLibrary.h"
#include "BTTask_FinishWithAIStatus.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTTask_FinishWithAIStatus : public UBTTaskNode
{
	GENERATED_BODY()	

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

public:
	UPROPERTY(EditAnywhere)
	EAIStatus AIStatus;
};
