// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GS_AIController.h"
#include "InstinctsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInstinctsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstinctsComponent();

	// Base Value for each
	UPROPERTY(EditAnywhere, Category = "Instincts")
	TMap<EAIGoal, float> BasicInstincts;

protected:
	virtual void BeginPlay() override;

private:
	void SortBasicInstinctsByPriority();
		
};
