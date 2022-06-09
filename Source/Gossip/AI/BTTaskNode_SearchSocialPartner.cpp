// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SearchSocialPartner.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "SocialComponent.h"

#include "Gossip/Characters/NonPlayerCharacter.h"

EBTNodeResult::Type UBTTaskNode_SearchSocialPartner::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	UActorComponent* SocialComponent = NPC->FindComponentByClass(USocialComponent::StaticClass());
	if (!SocialComponent) return EBTNodeResult::Failed;
	USocialComponent* SocialComp = Cast<USocialComponent>(SocialComponent);
	if (!SocialComp) return EBTNodeResult::Failed;

	AActor* TargetActor = SocialComp->FindSocialPartner();
	if (!TargetActor) return EBTNodeResult::Failed;

	NPC->SetMoveSpeed(1);
	BlackboardComp->SetValueAsObject("TargetActor", TargetActor);
	return EBTNodeResult::Succeeded;
}
