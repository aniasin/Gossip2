// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_InitiateSocialization.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "SocialComponent.h"


EBTNodeResult::Type UBTTaskNode_InitiateSocialization::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	APawn* NPC = OwnerComp.GetAIOwner()->GetPawn();
	if (!NPC) return EBTNodeResult::Failed;

	UActorComponent* SocialComponent = NPC->FindComponentByClass(USocialComponent::StaticClass());
	if (!SocialComponent) return EBTNodeResult::Failed;
	USocialComponent* SocialComp = Cast<USocialComponent>(SocialComponent);
	if (!SocialComp) return EBTNodeResult::Failed;

	AActor* OtherActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!IsValid(OtherActor)) return EBTNodeResult::Failed;

	if (!SocialComp->InitiateInteraction(OtherActor)) { BlackboardComp->ClearValue("TargetActor"); return EBTNodeResult::Failed;}

	AGS_AIController* OtherController = Cast<AGS_AIController>(OtherActor->GetInstigatorController());
	if (!IsValid(OtherController)) { BlackboardComp->ClearValue("TargetActor"); return EBTNodeResult::Failed; }

	BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Socialize);
	OtherController->GetBlackboardComponent()->SetValueAsEnum("Goal", (uint8)EAIGoal::None);
	OtherController->GetBlackboardComponent()->SetValueAsEnum("Action", (uint8)EAIAction::None);
	OtherController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NPC);
	OtherController->GetBlackboardComponent()->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Socialize);

	return EBTNodeResult::Succeeded;
}
