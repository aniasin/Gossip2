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
	FCollisionShape Sphere = FCollisionShape::MakeSphere(DistanceToTrace);
	//DrawDebugSphere(World, Start, Sphere.GetSphereRadius(), 50, FColor::Green, false, 10);
	
	bool bHit = (World->SweepMultiByChannel(Hits, Start, Start, FQuat::Identity, ECC_GameTraceChannel2, Sphere));

	ARessource* Ressource;
	for (FHitResult Hit : Hits)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap: %s"), *Hit.GetActor()->GetName())
		if (Action == EAIAction::SearchCollector && Hit.GetActor()->IsA(ARessourceCollector::StaticClass())
			|| Action == EAIAction::StockRaw && Hit.GetActor()->IsA(ARessourceCollector::StaticClass()))
		{
			Ressource = Cast<ARessource>(Hit.GetActor());
			InventoryComp->AddKnownRessourceCollector(Ressource);
			if (Ressource->RessourceType != Goal) break;

			AIController->SetTargetActor(Hit.GetActor());
			return EBTNodeResult::Succeeded;
		}
		if (Action == EAIAction::SearchProcessor && Hit.GetActor()->IsA(ARessourceProcessor::StaticClass()))
		{
			Ressource = Cast<ARessource>(Hit.GetActor());
			InventoryComp->AddKnownRessourceProcessor(Ressource);
			if (Ressource->RessourceType != Goal) break;

			AIController->SetTargetActor(Hit.GetActor());
			return EBTNodeResult::Succeeded;
		}
	}	
	return EBTNodeResult::Failed;
}
