// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsDistanceLess.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

bool UBTDecorator_IsDistanceLess::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) { return false; }

	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!OwnerPawn) return false;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!TargetActor)
	{
		FVector TargetLocation = BlackboardComp->GetValueAsVector("TargetLocation");
		return FVector::Distance(TargetLocation, OwnerPawn->GetActorLocation()) <= Distance;
	}
	return FVector::Distance(TargetActor->GetActorLocation(), OwnerPawn->GetActorLocation()) <= Distance;
}

FString UBTDecorator_IsDistanceLess::GetStaticDescription() const
{
	FString ReturnDesc = Super::GetStaticDescription();
	ReturnDesc += "\n\n";
	ReturnDesc += FString::Printf(TEXT("%s Distance is less than: %s"), *Target.SelectedKeyName.ToString(), *FString::SanitizeFloat(Distance));
	return ReturnDesc;
}
