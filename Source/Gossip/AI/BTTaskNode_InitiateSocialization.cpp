// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_InitiateSocialization.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GS_AIController.h"
#include "InstinctsComponent.h"
#include "SocialComponent.h"

#include "Gossip/Items/Ressource.h"
#include "Gossip/Characters/NonPlayerCharacter.h"


EBTNodeResult::Type UBTTaskNode_InitiateSocialization::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	AGS_AIController* AIController = Cast<AGS_AIController>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	UActorComponent* SocialComponent = NPC->FindComponentByClass(USocialComponent::StaticClass());
	if (!SocialComponent) return EBTNodeResult::Failed;
	USocialComponent* SocialComp = Cast<USocialComponent>(SocialComponent);
	if (!SocialComp) return EBTNodeResult::Failed;

	UActorComponent* InstinctComponent = NPC->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!InstinctComponent) return EBTNodeResult::Failed;
	UInstinctsComponent* InstinctComp = Cast<UInstinctsComponent>(InstinctComponent);
	if (!InstinctComp) return EBTNodeResult::Failed;

	AActor* OtherActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!IsValid(OtherActor)) return EBTNodeResult::Failed;

	UActorComponent* OtherSocialComponent = OtherActor->FindComponentByClass(USocialComponent::StaticClass());
	if (!OtherSocialComponent) return EBTNodeResult::Failed;
	USocialComponent* OtherSocialComp = Cast<USocialComponent>(OtherSocialComponent);
	if (!OtherSocialComp) return EBTNodeResult::Failed;

	AGS_AIController* OtherController = Cast<AGS_AIController>(OtherActor->GetInstigatorController());
	if (!IsValid(OtherController)) { BlackboardComp->ClearValue("TargetActor"); return EBTNodeResult::Failed; }

	EAIGoal Goal = (EAIGoal)BlackboardComp->GetValueAsEnum("Goal");

	NPC->SetMoveSpeed(0);

	if (OtherController->GetBlackboardComponent()->GetValueAsEnum("AIStatus") == (uint8)EAIStatus::LeadHome
		|| OtherController->GetBlackboardComponent()->GetValueAsEnum("AIStatus") == (uint8)EAIStatus::Follow
		|| OtherController->GetBlackboardComponent()->GetValueAsEnum("AIStatus") == (uint8)EAIStatus::CityHallCall
		|| OtherController->GetBlackboardComponent()->GetValueAsEnum("AIStatus") == (uint8)EAIStatus::Socialize
		|| OtherController->GetBlackboardComponent()->GetValueAsEnum("Goal") == (uint8)EAIGoal::Sleep)
	{
		AIController->ResetAI();
		return EBTNodeResult::Failed;
	}

	switch (Goal)
	{
	case EAIGoal::Jerk:
		SocialComp->InitiateInteraction(OtherActor, Goal);
		break;
	case EAIGoal::Sex:
		if (SocialComp->InitiateInteraction(OtherActor, Goal) && OtherSocialComp->CharacterProfile != SocialComp->CharacterProfile)
		{
			InstinctComp->SatisfyInstinct(Goal);
			BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::LeadHome);
			UE_LOG(LogTemp, Log, TEXT("%s Should Lead"), *NPC->GetName())

			OtherController->ResetAI();
			OtherController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NPC);
			OtherController->GetBlackboardComponent()->SetValueAsVector("TargetLocation", BlackboardComp->GetValueAsVector("HomeLocation"));
			OtherController->GetBlackboardComponent()->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Follow);
			OtherController->GetBlackboardComponent()->SetValueAsEnum("Goal", (uint8)EAIGoal::Sex);
			UE_LOG(LogTemp, Log, TEXT("%s Should follow"), *OtherActor->GetName())
			return EBTNodeResult::Succeeded;
		}
		break;
	}
	
	InstinctComp->SatisfyInstinct(Goal);
	BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Socialize);
	OtherController->ResetAI();
	OtherController->GetBlackboardComponent()->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Socialize);
	OtherController->GetBlackboardComponent()->SetValueAsObject("TargetActor", NPC);

	return EBTNodeResult::Succeeded;
}
