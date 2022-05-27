// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "GameFramework/Actor.h"

#include "Ressource.generated.h"

UCLASS()
class GOSSIP_API ARessource : public AActor
{
	GENERATED_BODY()
	
public:	
	ARessource();

	UPROPERTY(EditAnywhere)
	EAIGoal RessourceType;
	UPROPERTY(BlueprintReadOnly)
	bool bRaw;
	UPROPERTY(EditAnywhere)
	float WaitTime = 5;

	UPROPERTY(EditAnywhere)
	FString AnimMontage; //TODO Change to be a UAnimMontage

	virtual void CollectRessource(class UInventoryComponent* InventoryComp);

	float CurrentDistanceToQuerier;

protected:
	virtual void BeginPlay() override;

	

private:
	


};
