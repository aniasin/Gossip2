// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GS_AIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIGoalChangedSignature, int32, SpeedLevel);

UCLASS()
class GOSSIP_API AGS_AIController : public AAIController
{
	GENERATED_BODY()

	AGS_AIController();

public:
	class UAIPerceptionComponent* PerceptionComponent;
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;

	FGuid Id;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardData* BlackboardToSet;
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY()
	float SightRange = 4000;
	UPROPERTY()
	float LoseSightRange = 4500;
	UPROPERTY()
	float HearingRange = 2000;

	virtual void OnPossess(APawn* InPawn);

	UPROPERTY()
	FOnAIGoalChangedSignature OnAIGoalChanged;

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	void AIMoveToLocation(FVector Location);

	TArray<AActor*> GetCurrentlyPerceivedActors();

	void ResetAI();
	void RequestMoveToLocation(FVector Location, EAIStatus Reason);

	void SetTimeSearching();
	bool HasTimeSearchingElapsed();

private:
	bool bCanSeeHostile;
	float TimeSearching;

	void StartAi();
	
};
