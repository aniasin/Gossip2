// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ressource.h"
#include "RessourceCollector.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API ARessourceCollector : public ARessource
{
	GENERATED_BODY()
		ARessourceCollector();
public:

protected:

	virtual void CollectRessource(class UInventoryComponent* InventoryComp) override;
	virtual void AddRessourceAsKnown(class UInventoryComponent* InventoryComp) override;
	virtual void BeginPlay() override;
private:
	
};
