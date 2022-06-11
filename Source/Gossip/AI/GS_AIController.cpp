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
	// Creating Perception component
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

	// Assign the sight and hearing sense to the perception component
	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*HearingConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	// Configure range of sight and hearing sense
	SightConfig->SightRadius = SightRange;
	SightConfig->LoseSightRadius = LoseSightRange;
	HearingConfig->HearingRange = HearingRange;

}

void AGS_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		UseBlackboard(BlackboardToSet, BlackboardComponent);
	}
	BlackboardComponent->SetValueAsVector("HomeLocation", InPawn->GetActorLocation());
	BlackboardComponent->SetValueAsVector("TargetLocation", InPawn->GetActorLocation());
	
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGS_AIController::OnTargetPerceptionUpdate);

	RunBehaviorTree(BehaviorTree);
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
	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(TSubclassOf<UAISense_Sight>(), SeenActors);

	return SeenActors;
}

void AGS_AIController::SetAIGoal(uint8 Instinct)
{
	BlackboardComponent->SetValueAsEnum("Goal", Instinct);
}

void AGS_AIController::SetAIAction(uint8 Action)
{
	BlackboardComponent->SetValueAsEnum("Action", Action);
}

void AGS_AIController::ResetAI()
{
	BlackboardComponent->SetValueAsEnum("Goal", (uint8)EAIGoal::None);
	BlackboardComponent->SetValueAsEnum("Action", (uint8)EAIAction::None);
	BlackboardComponent->SetValueAsEnum("AIStatus", (uint8)EAIStatus::None);
}

uint8 AGS_AIController::GetAIGoal()
{
	return BlackboardComponent->GetValueAsEnum("Goal");
}

uint8 AGS_AIController::GetAIAction()
{
	return BlackboardComponent->GetValueAsEnum("Action");
}

void AGS_AIController::SetTimeSearching()
{
	TimeSearching = GetWorld()->GetTimeSeconds();
}

bool AGS_AIController::HasTimeSearchingElapsed()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return false;
	return GetWorld()->GetTimeSeconds() - TimeSearching >= GM->GameHourDurationSeconds;
}

void AGS_AIController::SetTargetActor(AActor* TargetActor)
{
	BlackboardComponent->SetValueAsObject("TargetActor", TargetActor);
}

void AGS_AIController::ClearTargetLocation()
{
	BlackboardComponent->ClearValue("TargetLocation");
}

