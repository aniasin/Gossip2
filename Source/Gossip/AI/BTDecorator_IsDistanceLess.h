// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsDistanceLess.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UBTDecorator_IsDistanceLess : public UBTDecorator
{
	GENERATED_BODY()
public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

protected:
	// Object or Vector
	UPROPERTY(VisibleAnywhere, Category = "AI")
		FBlackboardKeySelector Target;
	UPROPERTY(EditAnywhere, Category = "AI")
		float Distance;

};
