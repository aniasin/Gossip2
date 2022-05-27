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
			NewAction = (uint8)EAIAction::Stock;
			NewGoal = Stock(InstinctsComp);			
		}

		AIController->SetAIGoal(NewGoal);
		AIController->SetAIAction(NewAction);
		AIController->OnAIGoalChanged.Broadcast(bRun);
	}
}

uint8 UBTService_SetAIGoal::CheckGoal(float InstinctValue, EAIGoal Goal)
{
	if (InstinctValue > 0.8)
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

		return (uint8)EAIAction::SearchCollector;

	case EAIInstinct::Conservation:
		return (uint8)EAIAction::TravelProcessor;

	case EAIInstinct::Reproduction:
		break;
	default:
		break;
	}

	return (uint8)EAIInstinct::None;
}

uint8 UBTService_SetAIGoal::CheckTravelRoute(class UInventoryComponent* InventoryComp, uint8 NewAction, uint8 NewGoal, class AGS_AIController* AIController)
{
	if (NewAction == (uint8)EAIAction::TravelCollector)
	{
		AActor* TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)NewGoal);
		if (!TargetActor || !IsValid(TargetActor)) return (uint8)EAIAction::SearchCollector;
		if (IsValid(TargetActor)) AIController->SetTargetActor(TargetActor);
	}
	if (NewAction == (uint8)EAIAction::TravelProcessor)
	{
		AActor* TargetActor = InventoryComp->SearchNearestKnownRessourceProcessor((EAIGoal)NewGoal);
		if (!TargetActor || !IsValid(TargetActor)) return (uint8)EAIAction::SearchProcessor;
		if (IsValid(TargetActor)) AIController->SetTargetActor(TargetActor);
	}
	return NewAction;
}

uint8 UBTService_SetAIGoal::Stock(class UInstinctsComponent* InstinctsComp)
{
	return (uint8)InstinctsComp->InstinctsInfo[0].Goal;
}

