// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetAIGoal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AlignmentComponent.h"

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

	UActorComponent* Alignment = AIController->GetPawn()->FindComponentByClass(UAlignmentComponent::StaticClass());
	if (!Alignment) return;
	UAlignmentComponent* AlignmentComp = Cast<UAlignmentComponent>(Alignment);
	if (!AlignmentComp) return;

	uint8 PreviousGoal = AIController->GetAIGoal();
	uint8 NewGoal = (uint8)EAIGoal::None;
	uint8 NewAction = (uint8)EAIAction::None;

	for (EAIGoal Goal : TEnumRange<EAIGoal>())
	{
		float InventoryCount = *InventoryComp->BasicInstincts.Find((uint8)Goal);
		NewGoal = CheckGoal(InventoryCount, Goal);
		if (NewGoal != (uint8)EAIGoal::None)
		{
			NewAction = CheckAction(InventoryComp->GetKnownRessourcesCount((EAIGoal)NewGoal));
			break;
		}
	}

	if (NewGoal != PreviousGoal)
	{
		AIController->SetAIGoal(NewGoal, NewAction);
		AIController->OnAIGoalChanged.Broadcast(bRun);
	}
}

uint8 UBTService_SetAIGoal::CheckGoal(float InstinctValue, EAIGoal Goal)
{
	if (InstinctValue > 0)
	{
		return (uint8)Goal;
	}
	return (uint8)EAIGoal::None;
}

uint8 UBTService_SetAIGoal::CheckAction(int32 Inventory)
{
	if (Inventory < 1)
	{
		return (uint8)EAIAction::Search;
	}
	return (uint8)EAIAction::Satisfy;
}
