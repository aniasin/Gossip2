// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "Components/ActorComponent.h"

#include "InventoryComponent.generated.h"

class ARessource;

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	EAIGoal Ressource;
	UPROPERTY()
	bool bRaw;
	UPROPERTY()
	float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UFUNCTION(BlueprintCallable)
	TArray<ARessource*> SortRessourcesByDistance(TArray<ARessource*> RessourceToSort);

	void AddKnownRessourceCollector(ARessource* RessourceActor);
	int32 GetKnownRessourcesCollectorCount(EAIGoal RessourceType);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceCollector(EAIGoal RessourceType);

	void AddKnownRessourceProcessor(ARessource* RessourceActor);
	int32 GetKnownRessourcesProcessorCount(EAIGoal RessourceType);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceProcessor(EAIGoal RessourceType);

	void AddOwnedItem(EAIGoal RessourceType, bool bRaw);
	void RemoveOwnedItem(EAIGoal RessourceType, bool bRaw);
	int32 GetOwnedItemsCount(EAIGoal RessourceType, bool bRaw);

private:
	TArray<FInventoryItem> InventoryItems;
	TArray<ARessource*> KnownRessourcesCollector;
	TArray<ARessource*> KnownRessourcesProcessor;
	void ClearNullRessources(TArray<ARessource*> Array);
};
