// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


#include "Gossip/Core/GossipGameMode.h"



AGS_AIController::AGS_AIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("SightConfig"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(FName("HearingConfig"));

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->PeripheralVisionAngleDegrees = 90.00f;

	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectNeutrals = false;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	SightConfig->SightRadius = SightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	HearingConfig->HearingRange = HearingRange;

}

void AGS_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PerceptionComponent->UpdatePerceptionAllowList(SightConfig->GetSenseID(), false);
	PerceptionComponent->UpdatePerceptionAllowList(HearingConfig->GetSenseID(), false);

	BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		UseBlackboard(BlackboardToSet, BlackboardComponent);
	}
	BlackboardComponent->SetValueAsVector("HomeLocation", InPawn->GetActorLocation());
	BlackboardComponent->SetValueAsVector("TargetLocation", InPawn->GetActorLocation());
	
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGS_AIController::OnTargetPerceptionUpdate);

	FTimerHandle Timer;
	float RandomDelay = FMath::RandRange(1, 2);
	GetWorldTimerManager().SetTimer(Timer, this, &AGS_AIController::StartAi, 1, false, RandomDelay);
}

void AGS_AIController::OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus)
{
	TArray<AActor*> SeenActors;
	if (UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus) == SightConfig->GetSenseImplementation())
	{
		PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);
		bCanSeeHostile = Stimulus.WasSuccessfullySensed();
		if (bCanSeeHostile)
		{
			// Just seen Actor
		}
	}
	PerceptionComponent->UpdatePerceptionAllowList(SightConfig->GetSenseID(), false);
}

void AGS_AIController::AIMoveToLocation(FVector Location)
{
	// We don't want Player to hijack Socialize status.
	if (BlackboardComponent->GetValueAsEnum("AIStatus") == (uint8)EAIStatus::Socialize) return;

	BlackboardComponent->SetValueAsEnum("AIStatus", (uint8)EAIStatus::PlayerOrder);
	BlackboardComponent->SetValueAsVector("TargetLocation", Location);
}

TArray<AActor*> AGS_AIController::GetCurrentlyPerceivedActors()
{
	PerceptionComponent->UpdatePerceptionAllowList(SightConfig->GetSenseID(), true);
	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);
	return SeenActors;
}

void AGS_AIController::ResetAI()
{
	BlackboardComponent->SetValueAsEnum("Goal", (uint8)EAIGoal::None);
	BlackboardComponent->SetValueAsEnum("Action", (uint8)EAIAction::None);
	BlackboardComponent->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);
}

void AGS_AIController::SetTimeSearching()
{
	TimeSearching = GetWorld()->GetUnpausedTimeSeconds();
}

bool AGS_AIController::HasTimeSearchingElapsed()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return false;
	return GetWorld()->GetUnpausedTimeSeconds() - TimeSearching >= GM->GameHourDurationSeconds;
}

void AGS_AIController::StartAi()
{
	RunBehaviorTree(BehaviorTree);
}

