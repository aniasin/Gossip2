// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
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
	void ClearTargetLocation();

private:
	bool bCanSeeHostile;
	
};
