// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FinishReproduction.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "InstinctsComponent.h"

#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Data/DataLibrary.h"

EBTNodeResult::Type UBTTask_FinishReproduction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	UActorComponent* InstinctComponent = NPC->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!InstinctComponent) return EBTNodeResult::Failed;
	UInstinctsComponent* InstinctComp = Cast<UInstinctsComponent>(InstinctComponent);
	if (!InstinctComp) return EBTNodeResult::Failed;

	InstinctComp->SatisfyInstinct(EAIGoal::Sex);
	BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);
	BlackboardComp->SetValueAsEnum("Goal", (uint8)EAIGoal::None);
	BlackboardComp->SetValueAsEnum("Action", (uint8)EAIAction::None);
	BlackboardComp->ClearValue("TargetActor");
	return EBTNodeResult::Succeeded;
}
