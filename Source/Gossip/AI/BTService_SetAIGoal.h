// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "Gossip/Data/DataLibrary.h"
#include "BTService_SetAIGoal.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTService_SetAIGoal : public UBTService
{
	GENERATED_BODY()

	UBTService_SetAIGoal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bRun;

	uint8 CheckGoal(float InstinctValue, EAIGoal Goal);
	uint8 CheckAction(class UInventoryComponent* Inventory, class UInstinctsComponent* InstinctComp, uint8 NewGoal);
	uint8 CheckTravelRoute(class UInventoryComponent* InventoryComp, uint8 NewAction, uint8 NewGoal, class AGS_AIController* AIController);
	uint8 Stock(class UInstinctsComponent* InstinctsComp, UInventoryComponent* InventoryComp, AGS_AIController* AIController);
};
