// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_PerformAction.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "InstinctsComponent.h"
#include "SocialComponent.h"

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

	UActorComponent* SocialComponent = NPC->FindComponentByClass(USocialComponent::StaticClass());
	if (!SocialComponent) return EBTNodeResult::Failed;
	USocialComponent* SocialComp = Cast<USocialComponent>(SocialComponent);
	if (!SocialComp) return EBTNodeResult::Failed;

	EAIGoal Goal = (EAIGoal)(BlackboardComp->GetValueAsEnum(GoalKey.SelectedKeyName));
	EAIAction Action = (EAIAction)(BlackboardComp->GetValueAsEnum(ActionKey.SelectedKeyName));

	if (Action == EAIAction::Satisfy)
	{	
		InstinctsComp->SatisfyInstinct(Goal);
		InventoryComp->RemoveOwnedItem(Goal, false);
		BlackboardComp->SetValueAsFloat("WaitTime", 1);
		BlackboardComp->ClearValue("TargetActor");
		return EBTNodeResult::Succeeded;
	}

	if (Action == EAIAction::SearchProcessor || Action == EAIAction::SearchCollector || Action == EAIAction::TravelCollector || Action == EAIAction::TravelProcessor
		|| Action == EAIAction::StockRaw || Action == EAIAction::StockProcessed)
	{
		EAIInstinct CurrentInctinct = EAIInstinct::None;
		for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
		{
			if (Goal == Instinct.Goal)
			{
				CurrentInctinct = Instinct.Instinct;
			}
		}

		ARessource* Ressource = Cast<ARessource>(BlackboardComp->GetValueAsObject("TargetActor"));
		if (!Ressource) return EBTNodeResult::Failed;

		switch (CurrentInctinct)
		{
		case EAIInstinct::None:
			break;
		case EAIInstinct::Assimilation:
			Ressource->CollectRessource(InventoryComp);
			BlackboardComp->ClearValue("TargetActor");
			break;

		case EAIInstinct::Conservation:
			InstinctsComp->SatisfyInstinct(Goal);
			BlackboardComp->ClearValue("TargetActor");
			break;

		case EAIInstinct::Reproduction:	
			// Directly set in SetAIGoalAndAction
			break;
		}
		NPC->StopAnimMontage();
		BlackboardComp->SetValueAsFloat("WaitTime", 1);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
