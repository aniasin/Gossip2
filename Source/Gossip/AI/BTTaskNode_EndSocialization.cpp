// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_EndSocialization.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SocialComponent.h"
#include "GS_AIController.h"


EBTNodeResult::Type UBTTaskNode_EndSocialization::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AActor* OtherActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!IsValid(OtherActor)) return EBTNodeResult::Failed;

	APawn* NPC = OwnerComp.GetAIOwner()->GetPawn();
	if (!NPC) return EBTNodeResult::Failed;

	UActorComponent* SocialComponent = NPC->FindComponentByClass(USocialComponent::StaticClass());
	if (!SocialComponent) return EBTNodeResult::Failed;
	USocialComponent* SocialComp = Cast<USocialComponent>(SocialComponent);
	if (!SocialComp) return EBTNodeResult::Failed;

	SocialComp->EndInteraction(OtherActor);

	BlackboardComp->ClearValue("TargetActor");
	BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);

	return EBTNodeResult::Succeeded;
}
