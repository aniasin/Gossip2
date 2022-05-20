// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GS_AIController.generated.h"

UENUM(BlueprintType)
enum class EAIGoal : uint8 
{
	None       UMETA(DisplayName = "None"),
	Food       UMETA(DisplayName = "Food"),
	Sleep      UMETA(DisplayName = "Sleep"),
	Sex        UMETA(DisplayName = "Sex"),
};

UCLASS()
class GOSSIP_API AGS_AIController : public AAIController
{
	GENERATED_BODY()

	AGS_AIController();

public:
	class UAIPerceptionComponent* PerceptionComponent;
	class UAISenseConfig_Sight* SightConfig;
	class UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardData* BlackboardToSet;
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY()
	float SightRange = 2000;
	UPROPERTY()
	float LoseSightRange = 2500;
	UPROPERTY()
	float HearingRange = 1000;

	virtual void OnPossess(APawn* InPawn);

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	void AIMoveToLocation(FVector Location);
	void SetAIGoal(uint8 Value);
	uint8 GetAIGoal();

private:

	bool bCanSeeHostile;
	
};
