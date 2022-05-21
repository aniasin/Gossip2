// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetAIGoal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
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

	bool bRun = false;
	uint8 PreviousGoal = AIController->GetAIGoal();
	uint8 NewGoal = (uint8)EAIGoal::None;
	if (AlignmentComp->BasicInstincts.Feed > 0.8)
	{
		NewGoal = (uint8)EAIGoal::Food;
		bRun = true;
	}
	if (AlignmentComp->BasicInstincts.Sleep > 0.8)
	{
		NewGoal = (uint8)EAIGoal::Sleep;
	}
	if (AlignmentComp->BasicInstincts.Sex > 0.8)
	{
		NewGoal = (uint8)EAIGoal::Sex;
	}
	if (AlignmentComp->BasicInstincts.Feed > 0 && InventoryComp->GetKnownRessourcesCount(EAIGoal::SearchFood) < 1)
	{
		NewGoal = (uint8)EAIGoal::SearchFood;
	}
	if (AlignmentComp->BasicInstincts.Sleep > 0 && InventoryComp->GetKnownRessourcesCount(EAIGoal::SearchSleep) < 1)
	{
		NewGoal = (uint8)EAIGoal::SearchSleep;
	}
	if (AlignmentComp->BasicInstincts.Sex > 0 && InventoryComp->GetKnownRessourcesCount(EAIGoal::SearchSex) < 1)
	{
		NewGoal = (uint8)EAIGoal::SearchSex;
	}
	if (NewGoal != PreviousGoal)
	{
		AIController->SetAIGoal(NewGoal);
		AIController->OnAIGoalChanged.Broadcast(bRun);
	}
}

