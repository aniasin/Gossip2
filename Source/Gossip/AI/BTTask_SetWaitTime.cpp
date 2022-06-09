// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetWaitTime.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Gossip/Items/Ressource.h"

EBTNodeResult::Type UBTTask_SetWaitTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ARessource* Ressource = Cast<ARessource>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!Ressource) return EBTNodeResult::Failed;

	BlackboardComp->SetValueAsFloat("WaitTime", Ressource->WaitTime);
	return EBTNodeResult::Succeeded;
}
