// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FindRessourcesInRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"

#include "Gossip/Items/RessourceCollector.h"
#include "Gossip/Items/RessourceProcessor.h"
#include "Gossip/Items/Ressource.h"

EBTNodeResult::Type UBTTaskNode_FindRessourcesInRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AGS_AIController* AIController = Cast<AGS_AIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	EAIAction Action = (EAIAction)BlackboardComp->GetValueAsEnum(ActionKey.SelectedKeyName);

	// SphereTrace for Resources
	TArray<FHitResult> Hits;
	UWorld* World = AIController->GetWorld();
	FVector Start = AIController->GetPawn()->GetActorLocation();
	FVector Direction = AIController->GetPawn()->GetActorForwardVector();
	FVector End = Start + (Distance * Direction);
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Distance);
	DrawDebugSphere(GetWorld(), Start, Sphere.GetSphereRadius(), 50, FColor::Purple, true);

	if (World->SweepMultiByChannel(Hits, Start, Start, FQuat::Identity, ECC_GameTraceChannel2, Sphere))
	{
		for (FHitResult Hit : Hits)
		{
			if (Action == EAIAction::Search)
			{
				if (Hit.GetActor()->IsA(ARessourceCollector::StaticClass()))
				{
					BlackboardComp->SetValueAsObject(BBValueToSetKey.SelectedKeyName, Hit.GetActor());
					return EBTNodeResult::Succeeded;
				}
			}
			if (Action == EAIAction::Process)
			{
				if (Hit.GetActor()->IsA(ARessourceProcessor::StaticClass()))
				{
					BlackboardComp->SetValueAsObject(BBValueToSetKey.SelectedKeyName, Hit.GetActor());
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}
