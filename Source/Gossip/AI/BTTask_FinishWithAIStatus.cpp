// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FinishWithAIStatus.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"

EBTNodeResult::Type UBTTask_FinishWithAIStatus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AGS_AIController* AIController = Cast<AGS_AIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	AIController->ResetAI();
	BlackboardComp->SetValueAsEnum("AIStatus", (uint8)AIStatus);
	return EBTNodeResult::Succeeded;
}
