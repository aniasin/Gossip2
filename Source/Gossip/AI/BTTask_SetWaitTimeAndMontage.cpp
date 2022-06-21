// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetWaitTimeAndMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "Animation/AnimMontage.h" 

#include "Gossip/Items/Shelter.h"
#include "Gossip/Core/GossipGameMode.h"
#include "Gossip/Items/InventoryComponent.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Items/Ressource.h"

EBTNodeResult::Type UBTTask_SetWaitTimeAndMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// No Ressource
	if (!Ressource)
	{
		EAIGoal CurrentGoal = (EAIGoal)BlackboardComp->GetValueAsEnum("Goal");
		EAIAction CurrentAction = (EAIAction)BlackboardComp->GetValueAsEnum("Action");
		switch (CurrentGoal)
		{
		case EAIGoal::Food:
			BlackboardComp->SetValueAsFloat("WaitTime", 1);
			return EBTNodeResult::Succeeded;

		case EAIGoal::Shelter:
			AGossipGameMode* GM = Cast<AGossipGameMode>(NPC->GetWorld()->GetAuthGameMode());
			if (!GM) return EBTNodeResult::Failed;
			BlackboardComp->SetValueAsFloat("WaitTime", GM->GameHourDurationSeconds);
			AShelter* ShelterActor = Cast<AShelter>(BlackboardComp->GetValueAsObject("TargetActor"));
			if (!ShelterActor) return EBTNodeResult::Failed;
			ShelterActor->BeginConstruct();
			if (ShelterActor->ConstructMontage)
			{
				NPC->PlayAnimMontage(ShelterActor->ConstructMontage);
			}
			return EBTNodeResult::Succeeded;
		}
		return EBTNodeResult::Failed;
	}

	// Ressource
	if (Ressource->ContentCount <= 0)
	{
		InventoryComp->RemoveKnownRessourceCollector(Ressource);
		return EBTNodeResult::Failed;
	}

	UAnimMontage* AnimMontage = Ressource->GetAnimMontageMontage();
	if (AnimMontage)
	{
		NPC->PlayAnimMontage(AnimMontage);
		if (BlackboardComp->GetValueAsEnum("Goal") == (uint8)EAIGoal::Sleep)
		{
			NPC->SetActorLocation(Ressource->GetActorLocation());
			NPC->SetActorRotation(Ressource->GetActorRotation());
		}
	}

	BlackboardComp->SetValueAsFloat("WaitTime", Ressource->WaitTime);
	return EBTNodeResult::Succeeded;
}
