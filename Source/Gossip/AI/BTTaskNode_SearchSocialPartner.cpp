// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_SearchSocialPartner.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "SocialComponent.h"
#include "FamilyComponent.h"

#include "Gossip/Characters/NonPlayerCharacter.h"

EBTNodeResult::Type UBTTaskNode_SearchSocialPartner::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AGS_AIController* AIController = Cast<AGS_AIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;
	
	UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(NPC->GetComponentByClass(UFamilyComponent::StaticClass()));
	if (!FamilyComp) return EBTNodeResult::Failed;

	UActorComponent* SocialComponent = NPC->FindComponentByClass(USocialComponent::StaticClass());
	if (!SocialComponent) return EBTNodeResult::Failed;
	USocialComponent* SocialComp = Cast<USocialComponent>(SocialComponent);
	if (!SocialComp) return EBTNodeResult::Failed;

	EAIGoal Goal = (EAIGoal)BlackboardComp->GetValueAsEnum("Goal");

	bool bFriendly = true;
	AActor* TargetActor = nullptr;
	switch (Goal)
	{
	case EAIGoal::Sex:
		TargetActor = SocialComp->FindSocialPartner(bFriendly);
		if (!TargetActor)
		{
			return EBTNodeResult::Failed;
		}
		NPC->SetMoveSpeed(1);
		BlackboardComp->SetValueAsObject("TargetActor", TargetActor);
		return EBTNodeResult::Succeeded;

	case EAIGoal::Jerk:
		bFriendly = false;
		TargetActor = SocialComp->FindSocialPartner(bFriendly);
		if (!TargetActor)
		{
			return EBTNodeResult::Failed;
		}
		NPC->SetMoveSpeed(1);
		BlackboardComp->SetValueAsObject("TargetActor", TargetActor);
		return EBTNodeResult::Succeeded;

	case EAIGoal::Children:
		TArray<AActor*>Spouses = FamilyComp->GetSpouses();
		if (Spouses.IsEmpty())
		{
			return EBTNodeResult::Failed;
		}
		NPC->SetMoveSpeed(1);
		int32 RandomSpouse = FMath::RandRange(0, Spouses.Num() - 1);
		BlackboardComp->SetValueAsObject("TargetActor", Spouses[RandomSpouse]);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
