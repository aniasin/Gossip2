// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FinishWithSetEnum.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_FinishWithSetEnum::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	BlackboardComp->SetValueAsEnum(EnumToSetKey.SelectedKeyName, (uint8)EnumValue);
	return EBTNodeResult::Succeeded;
}
