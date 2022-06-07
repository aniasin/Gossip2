// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_InitiateSocialization.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "SocialComponent.h"
#include "InstinctsComponent.h"

#include "Gossip/Items/Ressource.h"


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

	UActorComponent* InstinctComponent = NPC->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!InstinctComponent) return EBTNodeResult::Failed;
	UInstinctsComponent* InstinctComp = Cast<UInstinctsComponent>(InstinctComponent);
	if (!InstinctComp) return EBTNodeResult::Failed;

	AActor* OtherActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!IsValid(OtherActor)) return EBTNodeResult::Failed;

	UActorComponent* OtherSocialComponent = OtherActor->FindComponentByClass(USocialComponent::StaticClass());
	if (!OtherSocialComponent) return EBTNodeResult::Failed;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(OtherSocialComponent);
	if (!OtherSocialComp) return EBTNodeResult::Failed;

	AGS_AIController* OtherController = Cast<AGS_AIController>(OtherActor->GetInstigatorController());
	if (!IsValid(OtherController)) { BlackboardComp->ClearValue("TargetActor"); return EBTNodeResult::Failed; }

	int32 Proximity = SocialComp->InitiateInteraction(OtherActor);

	if (BlackboardComp->GetValueAsEnum("Goal") == (uint8)EAIGoal::Sex)
	{
		InstinctComp->SatisfyInstinct(EAIGoal::Sex);
		if (Proximity >= 10)
		{
			BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::LeadHome);
			UE_LOG(LogTemp, Log, TEXT("%s Should Lead"), *NPC->GetName())

			OtherController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NPC);
			OtherController->GetBlackboardComponent()->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Follow);
			UE_LOG(LogTemp, Log, TEXT("%s Should follow"), *OtherActor->GetName())
			return EBTNodeResult::Succeeded;
		}
	}

	BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Socialize);
	OtherController->GetBlackboardComponent()->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Socialize);
	OtherController->GetBlackboardComponent()->SetValueAsEnum("Goal", (uint8)EAIGoal::None);
	OtherController->GetBlackboardComponent()->SetValueAsEnum("Action", (uint8)EAIAction::None);
	OtherController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NPC);


	return EBTNodeResult::Succeeded;
}
