// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"



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

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGS_AIController::OnTargetPerceptionUpdate);
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
			UE_LOG(LogTemp, Warning, TEXT("Can see %s"), *Actor->GetName());
		}
	}
}
