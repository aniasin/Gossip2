// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FinishReproduction.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GS_AIController.h"
#include "InstinctsComponent.h"

#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Characters/PlayerPawn.h"
#include "Gossip/Characters/SocialRulesComponent.h"
#include "Gossip/Data/DataLibrary.h"

EBTNodeResult::Type UBTTask_FinishReproduction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	UActorComponent* InstinctComponent = NPC->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!InstinctComponent) return EBTNodeResult::Failed;
	UInstinctsComponent* InstinctComp = Cast<UInstinctsComponent>(InstinctComponent);
	if (!InstinctComp) return EBTNodeResult::Failed;

	InstinctComp->SatisfyInstinct(EAIGoal::Sex);

	AActor* PlayerActor = UGameplayStatics::GetActorOfClass(NPC->GetWorld(), APlayerPawn::StaticClass());
	UActorComponent* Component = PlayerActor->GetComponentByClass(USocialRulesComponent::StaticClass());
	USocialRulesComponent* SocialRulesComp = Cast<USocialRulesComponent>(Component);

	if (!BlackboardComp->GetValueAsObject("TargetActor"))
	{
		BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);
		return EBTNodeResult::Failed;
	}
	AActor* Other = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	TMap<AActor*, AActor*>Couple;
	if (!Other)
	{
		BlackboardComp->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);
		return EBTNodeResult::Failed;
	}

	if (bLeader)
	{
		Couple.Add(NPC, Other);
		SocialRulesComp->NewWeddingCandidates(Couple);
	}

	return EBTNodeResult::InProgress;
}
