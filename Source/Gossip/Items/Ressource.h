// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gossip/AI/GS_AIController.h"
#include "Ressource.generated.h"

UCLASS()
class GOSSIP_API ARessource : public AActor
{
	GENERATED_BODY()
	
public:	
	ARessource();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIGoal RessourceType;

protected:
	virtual void BeginPlay() override;

private:

};
