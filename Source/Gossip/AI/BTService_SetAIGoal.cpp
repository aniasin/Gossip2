// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetAIGoal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstinctsComponent.h"
#include "GS_AIController.h"

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
	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		float InstinctValue = FMath::Abs(Instinct.CurrentValue);
		NewGoal = CheckGoal(InstinctValue, Instinct.Goal);

		if (NewGoal != (uint8)EAIGoal::None)
		{
			NewAction = CheckAction(InventoryComp, InstinctsComp, NewGoal);
			break;
		}
	}
	// New Goal Found or new action
	if (NewGoal != PreviousGoal || NewAction != PreviousAction)
	{
		NewAction = CheckTravelRoute(InventoryComp, NewAction, NewGoal, AIController);	

		if (NewGoal == ((uint8)EAIGoal::None))
		{
			uint8 ActionToSet;
			NewGoal = CheckStock(InstinctsComp, InventoryComp, AIController, ActionToSet);
			NewAction = ActionToSet;
			// TODO Goal is still none: entertainment.
		}

		AIController->SetAIGoal(NewGoal);
		AIController->SetAIAction(NewAction);
		AIController->OnAIGoalChanged.Broadcast(bRun);
	}
}

uint8 UBTService_SetAIGoal::CheckGoal(float InstinctValue, EAIGoal Goal)
{
	if (InstinctValue > .8)
	{
		return (uint8)Goal;
	}
	return (uint8)EAIGoal::None;
}

uint8 UBTService_SetAIGoal::CheckAction(class UInventoryComponent* Inventory, UInstinctsComponent* InstinctsComp, uint8 NewGoal)
{
	EAIInstinct NewInstinct = EAIInstinct::None;

	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		if (Instinct.Goal == (EAIGoal)NewGoal)
		{
			NewInstinct = Instinct.Instinct;
		}
	}	

	int32 OwnedRessourceProcessed = 0;
	int32 OwnedRessourceRaw = 0;

	switch (NewInstinct)
	{
	case EAIInstinct::None:
		break;
	case EAIInstinct::Assimilation:
		OwnedRessourceProcessed = Inventory->GetOwnedItemsCount((EAIGoal)NewGoal, false);
		if (OwnedRessourceProcessed > 0) { return (uint8)EAIAction::Satisfy; }

		OwnedRessourceRaw = Inventory->GetOwnedItemsCount((EAIGoal)NewGoal, true);
		if (OwnedRessourceRaw > 0) { return (uint8)EAIAction::TravelProcessor; }

		return (uint8)EAIAction::TravelCollector;

	case EAIInstinct::Conservation:
		return (uint8)EAIAction::TravelCollector;

	case EAIInstinct::Reproduction:
		break;
	default:
		break;
	}

	return (uint8)EAIInstinct::None;
}

uint8 UBTService_SetAIGoal::CheckTravelRoute(UInventoryComponent* InventoryComp, uint8 NewAction, uint8 NewGoal, class AGS_AIController* AIController)
{
	if (NewAction == (uint8)EAIAction::TravelCollector)
	{
		AActor* TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)NewGoal);
		if (!IsValid(TargetActor)) return (uint8)EAIAction::SearchCollector;

		AIController->SetTargetActor(TargetActor);
		return (uint8)EAIAction::TravelCollector;
	}
	if (NewAction == (uint8)EAIAction::TravelProcessor)
	{
		AActor* TargetActor = InventoryComp->SearchNearestKnownRessourceProcessor((EAIGoal)NewGoal);
		if (!IsValid(TargetActor)) return (uint8)EAIAction::SearchProcessor;

		AIController->SetTargetActor(TargetActor);
		return (uint8)EAIAction::TravelProcessor;
	}
	return NewAction;
}

uint8 UBTService_SetAIGoal::CheckStock(class UInstinctsComponent* InstinctsComp, UInventoryComponent* InventoryComp, AGS_AIController* AIController, uint8& ActionToSet)
{
	AActor* TargetActor = nullptr;
	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		if (Instinct.bStockable == false) break;
		if (InventoryComp->GetOwnedItemsCount(Instinct.Goal, true) > 0 && InventoryComp->GetOwnedItemsCount(Instinct.Goal, false) < 10) // TODO Make variable
		{
			ActionToSet = (uint8)EAIAction::StockProcessed;
			TargetActor = InventoryComp->SearchNearestKnownRessourceProcessor((EAIGoal)Instinct.Goal);
			if (!IsValid(TargetActor)) AIController->SetTargetActor(TargetActor);
			return (uint8)Instinct.Goal;
		}
		if (InventoryComp->GetOwnedItemsCount(Instinct.Goal, true) < 10) // TODO Make variable
		{
			ActionToSet = (uint8)EAIAction::StockRaw;
			TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)Instinct.Goal);
			if (!IsValid(TargetActor)) AIController->SetTargetActor(TargetActor);			
			return (uint8)Instinct.Goal;
		}
	}
	ActionToSet = (uint8)EAIAction::None;
	return (uint8)EAIGoal::None;	
}

