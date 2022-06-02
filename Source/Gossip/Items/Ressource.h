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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class URessourceDataAsset* RessourceData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAIGoal RessourceType;
	UPROPERTY(BlueprintReadWrite)
	bool bRaw;
	UPROPERTY(BlueprintReadWrite)
	float WaitTime = 5;
	UPROPERTY(EditAnywhere)
	FString AnimMontage; //TODO Change to be a UAnimMontage

	virtual void CollectRessource(class UInventoryComponent* InventoryComp);
	virtual void AddRessourceAsKnown(class UInventoryComponent* InventoryComp);

	float CurrentDistanceToQuerier;

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;

	virtual void BeginPlay() override;

	

private:
	


};
