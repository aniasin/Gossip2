// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "Gossip/Data/DataLibrary.h"
#include "BTService_SetAIGoalAndAction.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTService_SetAIGoalAndAction : public UBTService
{
	GENERATED_BODY()

	UBTService_SetAIGoalAndAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool InitializeService(UBehaviorTreeComponent& OwnerComp);

private:
	bool bRun;

	class AGS_AIController* AIController;
	class UInventoryComponent* InventoryComp;
	class UInstinctsComponent* InstinctsComp;
	uint8 PreviousGoal;
	uint8 PreviousAction;
	uint8 NewGoal;
	uint8 NewAction;

	void SetGoalAndAction();
	void SetAction();
	void SetTravelRoute();
	void CheckStock();
};
