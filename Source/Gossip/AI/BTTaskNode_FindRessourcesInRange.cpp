// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FindRessourcesInRange.h"
#include "Gossip/Data/DataLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"

#include "Gossip/Items/InventoryComponent.h"
#include "Gossip/Items/RessourceCollector.h"
#include "Gossip/Items/RessourceProcessor.h"
#include "Gossip/Items/Ressource.h"


EBTNodeResult::Type UBTTaskNode_FindRessourcesInRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AGS_AIController* AIController = Cast<AGS_AIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	UActorComponent* Inventory = AIController->GetPawn()->FindComponentByClass(UInventoryComponent::StaticClass());
	if (!Inventory) return EBTNodeResult::Failed;
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(Inventory);
	if (!InventoryComp) return EBTNodeResult::Failed;

	EAIAction Action = (EAIAction)BlackboardComp->GetValueAsEnum(ActionKey.SelectedKeyName);
	EAIGoal Goal = (EAIGoal)BlackboardComp->GetValueAsEnum(GoalKey.SelectedKeyName);

	// SphereTrace for Resources
	TArray<FHitResult> Hits;
	UWorld* World = AIController->GetWorld();
	FVector Start = AIController->GetPawn()->GetActorLocation();
	FVector Direction = AIController->GetPawn()->GetActorForwardVector();
	FVector End = Start + (DistanceToTrace * Direction);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(2000);
	//DrawDebugSphere(World, Start, Sphere.GetSphereRadius(), 50, FColor::Green, false, 10);
	
	bool bHit = (World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_GameTraceChannel2, Sphere));

	ARessource* Ressource;
	for (FHitResult Hit : Hits)
	{
		Ressource = Cast<ARessource>(Hit.GetActor());
		if (Ressource->IsAttachedTo(AIController->GetPawn())) continue;
		if (Ressource->Owners.Num() > 0 && !Ressource->Owners.Contains(AIController->GetPawn())) continue;

		if (Action == EAIAction::SearchCollector && Hit.GetActor()->IsA(ARessourceCollector::StaticClass())
			|| Action == EAIAction::StockRaw && Hit.GetActor()->IsA(ARessourceCollector::StaticClass()))
		{
			if (Ressource->ContentCount <= 0)
			{
				InventoryComp->RemoveKnownRessourceCollector(Ressource);
				continue;
			}
			InventoryComp->AddKnownRessourceCollector(Ressource);
			if (Ressource->RessourceType != Goal) continue;

			AIController->SetTargetActor(Ressource);
			return EBTNodeResult::Succeeded;
		}
		if (Action == EAIAction::SearchProcessor && Hit.GetActor()->IsA(ARessourceProcessor::StaticClass())
			|| Action == EAIAction::StockProcessed && Hit.GetActor()->IsA(ARessourceProcessor::StaticClass()))
		{
			if (Ressource->RessourceType != Goal) continue;

			AIController->SetTargetActor(Ressource);
			return EBTNodeResult::Succeeded;
		}
	}	
	return EBTNodeResult::Failed;
}
