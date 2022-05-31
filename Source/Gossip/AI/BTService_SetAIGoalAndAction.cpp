// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetAIGoalAndAction.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstinctsComponent.h"
#include "GS_AIController.h"

#include "Gossip/Items/InventoryComponent.h"

UBTService_SetAIGoalAndAction::UBTService_SetAIGoalAndAction(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_SetAIGoalAndAction::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	
}

void UBTService_SetAIGoalAndAction::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) { return; }
	
}

void UBTService_SetAIGoalAndAction::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!InitializeService(OwnerComp)) return;
	//UE_LOG(LogTemp, Log, TEXT("SERVICE SetAIGoalAndAction"))

	// Check something needed now & Take action
	SetGoalAndAction();	

	CheckStock();
	// TODO Goal is still none: entertainment.
	SetTravelRoute();

	AIController->SetAIGoal(NewGoal);
	AIController->SetAIAction(NewAction);
	AIController->OnAIGoalChanged.Broadcast(bRun);

}

//////////////////////////////////////////////////////////////
bool UBTService_SetAIGoalAndAction::InitializeService(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return false;
	AIController = Cast<AGS_AIController>(Controller);
	if (!AIController) return false;

	UActorComponent* Inventory = AIController->GetPawn()->FindComponentByClass(UInventoryComponent::StaticClass());
	if (!Inventory) return false;
	InventoryComp = Cast<UInventoryComponent>(Inventory);
	if (!InventoryComp) return false;

	UActorComponent* Instincts = AIController->GetPawn()->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!Instincts) return false;
	InstinctsComp = Cast<UInstinctsComponent>(Instincts);
	if (!InstinctsComp) return false;

	PreviousGoal = AIController->GetAIGoal();
	PreviousAction = AIController->GetAIAction();
	NewGoal = (uint8)EAIGoal::None;
	NewAction = (uint8)EAIAction::None;

	return true;
}

void UBTService_SetAIGoalAndAction::SetGoalAndAction()
{
	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		float InstinctValue = FMath::Abs(Instinct.CurrentValue);
		if (InstinctValue > .8)
		{
			NewGoal = (uint8)Instinct.Goal;
			break;
		}
		NewGoal = (uint8)EAIGoal::None;
	}
	if (NewGoal != (uint8)EAIGoal::None)
	{
		SetAction();
	}

}

void UBTService_SetAIGoalAndAction::SetAction()
{
	EAIInstinct NewInstinct = EAIInstinct::None;

	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		if (Instinct.Goal == (EAIGoal)NewGoal)
		{
			NewInstinct = Instinct.Instinct;
			break;
		}
	}	

	int32 OwnedRessourceProcessed = 0;
	int32 OwnedRessourceRaw = 0;

	switch (NewInstinct)
	{
	case EAIInstinct::None:
		break;
	case EAIInstinct::Assimilation:
		OwnedRessourceProcessed = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, false);
		if (OwnedRessourceProcessed > 0) { NewAction = (uint8)EAIAction::Satisfy;  return; }

		OwnedRessourceRaw = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, true);
		if (OwnedRessourceRaw > 0) { NewAction = (uint8)EAIAction::TravelProcessor;  return; }

		NewAction = (uint8)EAIAction::TravelCollector;
		return;

	case EAIInstinct::Conservation:
		NewAction = (uint8)EAIAction::TravelCollector;
		return;

	case EAIInstinct::Reproduction:
		return;
	}

	NewAction = (uint8)EAIInstinct::None;
}

void UBTService_SetAIGoalAndAction::SetTravelRoute()
{
	if (NewAction == (uint8)EAIAction::TravelCollector)
	{
		TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)NewGoal);
		if (!IsValid(TargetActor)) { NewAction = (uint8)EAIAction::SearchCollector; return;	}
		AIController->SetTargetActor(TargetActor);
		NewAction = (uint8)EAIAction::TravelCollector;
		return;
	}
	if (NewAction == (uint8)EAIAction::TravelProcessor)
	{
		TargetActor = InventoryComp->SearchNearestKnownRessourceProcessor((EAIGoal)NewGoal);
		if (!IsValid(TargetActor)) { NewAction = (uint8)EAIAction::SearchProcessor;	return;	}
		AIController->SetTargetActor(TargetActor);
		NewAction = (uint8)EAIAction::TravelProcessor;
	}
}

void UBTService_SetAIGoalAndAction::CheckStock()
{
	if (NewGoal != ((uint8)EAIGoal::None)) return;

	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		if (Instinct.bStockable == false) break;
		if (InventoryComp->GetOwnedItemsCount(Instinct.Goal, true) < 3 && PreviousAction != (uint8)EAIAction::StockProcessed
			|| InventoryComp->GetOwnedItemsCount(Instinct.Goal, true) < 3 && InventoryComp->GetOwnedItemsCount(Instinct.Goal, false) >= 3) 
			// TODO Make variable Desired Stock Raw and Desired Stock Processed
		{
			NewAction = (uint8)EAIAction::StockRaw;
			TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)Instinct.Goal);
			if (IsValid(TargetActor)) AIController->SetTargetActor(TargetActor);
			NewGoal = (uint8)Instinct.Goal;
			return;
		}
		if (InventoryComp->GetOwnedItemsCount(Instinct.Goal, true) >= 0 && InventoryComp->GetOwnedItemsCount(Instinct.Goal, false) < 3)
		{
			NewAction = (uint8)EAIAction::StockProcessed;
			TargetActor = InventoryComp->SearchNearestKnownRessourceProcessor((EAIGoal)Instinct.Goal);
			if (IsValid(TargetActor)) AIController->SetTargetActor(TargetActor);
			NewGoal = (uint8)Instinct.Goal;
			return;
		}

	}
	NewAction = (uint8)EAIAction::None;
	NewGoal = (uint8)EAIGoal::None;	
}
