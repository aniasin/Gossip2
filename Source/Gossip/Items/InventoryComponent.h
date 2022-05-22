// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/AI/GS_AIController.h"
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

	void AddKnownRessource(ARessource* RessourceActor);
	int32 GetKnownRessourcesCount(EAIGoal RessourceType);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessource(EAIGoal RessourceType);

	void AddOwnedItem(FInventoryItem Item);
	int32 GetOwnedItemsCount(EAIGoal RessourceType, bool bRaw);

private:
	TArray<FInventoryItem> InventoryItems;
	TArray<ARessource*> KnownRessources;

};
