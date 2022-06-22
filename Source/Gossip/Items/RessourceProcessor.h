// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ressource.h"
#include "RessourceProcessor.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API ARessourceProcessor : public ARessource
{
	GENERATED_BODY()
		ARessourceProcessor();
public:


protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR
	virtual void CollectRessource(class UInventoryComponent* InventoryComp, AActor* Actor) override;
	virtual void AddRessourceAsKnown(class UInventoryComponent* InventoryComp)override;
	virtual void BeginPlay() override;

private:
};
