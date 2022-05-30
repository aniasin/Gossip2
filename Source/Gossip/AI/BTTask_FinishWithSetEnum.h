// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_FinishWithResult.h"

#include "Gossip/Data/DataLibrary.h"
#include "BTTask_FinishWithSetEnum.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTTask_FinishWithSetEnum : public UBTTask_FinishWithResult
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;

protected:
	UPROPERTY(VisibleAnywhere)
	FBlackboardKeySelector EnumToSetKey;
	UPROPERTY(EditAnywhere)
	EAIStatus EnumValue;
};
