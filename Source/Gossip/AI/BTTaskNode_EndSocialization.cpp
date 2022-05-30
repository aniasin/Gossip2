// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_EndSocialization.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"


EBTNodeResult::Type UBTTaskNode_EndSocialization::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AActor* OtherActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!IsValid(OtherActor)) return EBTNodeResult::Failed;

	AGS_AIController* OtherController = Cast<AGS_AIController>(OtherActor->GetInstigatorController());
	if (!IsValid(OtherController)) { BlackboardComp->ClearValue("TargetActor"); return EBTNodeResult::Failed; }

	BlackboardComp->ClearValue("TargetActor");
	BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);

	OtherController->GetBlackboardComponent()->ClearValue("TargetActor");
	OtherController->GetBlackboardComponent()->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);

	return EBTNodeResult::Succeeded;
}
