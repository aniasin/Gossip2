// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GS_AIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAIGoalChangedSignature, bool, bRun);

UENUM(BlueprintType)
enum class EAIStatus : uint8
{
	None				UMETA(DisplayName = "None"),
	Collecting			UMETA(DisplayName = "Collecting"),
	Processing			UMETA(DisplayName = "Processing"),
	Satisfying			UMETA(DisplayName = "Satisfying"),
};

UENUM(BlueprintType)
enum class EAIGoal : uint8 
{
	None				UMETA(DisplayName = "None"),
	Food				UMETA(DisplayName = "Food"),
	Sleep				UMETA(DisplayName = "Sleep"),
	Sex					UMETA(DisplayName = "Sex"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAIGoal, EAIGoal::Food, EAIGoal::Sex);

UENUM(BlueprintType)
enum class EAIAction : uint8
{
	None				UMETA(DisplayName = "None"),
	SearchCollector		UMETA(DisplayName = "SearchCollector"),
	Stock				UMETA(DisplayName = "Stock"),
	Satisfy				UMETA(DisplayName = "Satisfy"),
	SearchProcessor		UMETA(DisplayName = "SearchProcessor"),
	TravelCollector		UMETA(DisplayName = "TravelCollector"),
	TravelProcessor		UMETA(DisplayName = "TravelProcessor"),
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

	//For Debug
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAIGoal CurrentGoal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EAIAction CurrentAction;

	virtual void OnPossess(APawn* InPawn);

	UPROPERTY()
	FOnAIGoalChangedSignature OnAIGoalChanged;

	UFUNCTION()
	void OnTargetPerceptionUpdate(AActor* Actor, FAIStimulus Stimulus);

	void AIMoveToLocation(FVector Location);
	void SetAIGoal(uint8 Instinct);
	void SetAIAction(uint8 Action);
	uint8 GetAIGoal();
	uint8 GetAIAction();

	void SetTargetActor(AActor* TargetActor);

private:
	bool bCanSeeHostile;
	
};
