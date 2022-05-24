// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_PerformAction.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "InstinctsComponent.h"

#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Items/InventoryComponent.h"
#include "Gossip/Items/Ressource.h"


EBTNodeResult::Type UBTTaskNode_PerformAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	UActorComponent* InstinctsComponent = NPC->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!InstinctsComponent) return EBTNodeResult::Failed;
	UInstinctsComponent* InstinctsComp = Cast<UInstinctsComponent>(InstinctsComponent);
	if (!InstinctsComp) return EBTNodeResult::Failed;

	UActorComponent* InventoryComponent = NPC->FindComponentByClass(UInventoryComponent::StaticClass());
	if (!InventoryComponent) return EBTNodeResult::Failed;
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(InventoryComponent);
	if (!InventoryComp) return EBTNodeResult::Failed;

	EAIAction Action = (EAIAction)(BlackboardComp->GetValueAsEnum(ActionKey.SelectedKeyName));
	if (Action == EAIAction::Satisfy)
	{
		EAIGoal Goal = (EAIGoal)(BlackboardComp->GetValueAsEnum(GoalKey.SelectedKeyName));
		InstinctsComp->SatisfyInstinct(Goal);
		return EBTNodeResult::Succeeded;
	}
	if (Action == EAIAction::Process || Action == EAIAction::Search)
	{
		UE_LOG(LogTemp, Warning, TEXT("Perform Action"))
		EAIGoal Goal = (EAIGoal)(BlackboardComp->GetValueAsEnum(GoalKey.SelectedKeyName));
		ARessource* Ressource = Cast<ARessource>(BlackboardComp->GetValueAsObject("TargetActor"));
		if (!Ressource) return EBTNodeResult::Failed;
		Ressource->CollectRessource(InventoryComp);
		BlackboardComp->ClearValue("TargetActor");
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Succeeded;
}
