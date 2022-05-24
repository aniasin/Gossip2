// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetAIGoal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstinctsComponent.h"

#include "Gossip/Items/InventoryComponent.h"

UBTService_SetAIGoal::UBTService_SetAIGoal(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_SetAIGoal::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) { return; }
}

void UBTService_SetAIGoal::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return;
	AGS_AIController* AIController = Cast<AGS_AIController>(Controller);
	if (!AIController) return;

	UActorComponent* Inventory = AIController->GetPawn()->FindComponentByClass(UInventoryComponent::StaticClass());
	if (!Inventory) return;
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(Inventory);
	if (!InventoryComp) return;

	UActorComponent* Instincts = AIController->GetPawn()->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!Instincts) return;
	UInstinctsComponent* InstinctsComp = Cast<UInstinctsComponent>(Instincts);
	if (!InstinctsComp) return;

	uint8 PreviousGoal = AIController->GetAIGoal();
	uint8 PreviousAction = AIController->GetAIAction();
	uint8 NewGoal = (uint8)EAIGoal::None;
	uint8 NewAction = (uint8)EAIAction::None;

	// Check something needed now & Take action
	for (auto& Goal : InstinctsComp->BasicInstincts)
	{
		float InstinctValue = Goal.Value.CurrentValue;
		NewGoal = CheckGoal(InstinctValue, Goal.Key);
		if (NewGoal != (uint8)EAIGoal::None)
		{
			int32 OwnedRessourceProcessed = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, false);
			if (OwnedRessourceProcessed > 0) { NewAction = (uint8)EAIAction::Satisfy; break; }
			
			int32 OwnedRessourceRaw = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, true);
			if (OwnedRessourceRaw > 0) { NewAction = (uint8)EAIAction::Process; break; }

			int32 KnownRessourceCount = InventoryComp->GetKnownRessourcesCollectorCount((EAIGoal)NewGoal);
			if (KnownRessourceCount > 0) { NewAction = (uint8)EAIAction::Travel;  break; }

			if (KnownRessourceCount <= 0) { NewAction = (uint8)EAIAction::Search; break; }
		}
	}
	// New Goal Found or new action
	if (NewGoal != PreviousGoal || NewAction != PreviousAction)
	{
		if (NewAction == (uint8)EAIAction::Travel)
		{
			AActor* TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)NewGoal);
			AIController->SetTargetActor(TargetActor);
		}
		AIController->SetAIGoal(NewGoal);
		AIController->SetAIAction(NewAction);
		AIController->OnAIGoalChanged.Broadcast(bRun);
	}
// 	// No new Goal 
// 	if (NewGoal == PreviousGoal)
// 	{
// 		// TODO Check for Stock needed
// 	}

}

uint8 UBTService_SetAIGoal::CheckGoal(float InstinctValue, EAIGoal Goal)
{
	if (InstinctValue > 0.8)
	{
		return (uint8)Goal;
	}
	return (uint8)EAIGoal::None;
}

