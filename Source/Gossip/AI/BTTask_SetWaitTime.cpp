// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetWaitTime.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"

#include "Gossip/Items/InventoryComponent.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Items/Ressource.h"

EBTNodeResult::Type UBTTask_SetWaitTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	UActorComponent* InventoryComponent = NPC->FindComponentByClass(UInventoryComponent::StaticClass());
	if (!InventoryComponent) return EBTNodeResult::Failed;
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(InventoryComponent);
	if (!InventoryComp) return EBTNodeResult::Failed;

	ARessource* Ressource = Cast<ARessource>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!Ressource)
	{
		BlackboardComp->SetValueAsFloat("WaitTime", 1);
		return EBTNodeResult::Succeeded;
	}

	if (Ressource->ContentCount <= 0)
	{
		InventoryComp->RemoveKnownRessourceCollector(Ressource);
		return EBTNodeResult::Failed;
	}
	BlackboardComp->SetValueAsFloat("WaitTime", Ressource->WaitTime);
	return EBTNodeResult::Succeeded;
}
