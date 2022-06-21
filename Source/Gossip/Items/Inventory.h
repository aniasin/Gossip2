// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gossip/Data/DataLibrary.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // For testing, should be private
	int32 StockingLimit = 10;

	void AddOwnedItem(EAIGoal RessourceType, ERessourceSubType RessourceSubType, bool bRaw);
	void RemoveOwnedItem(EAIGoal RessourceType, ERessourceSubType RessourceSubType, bool bRaw);
	int32 GetOwnedItemsCount(EAIGoal RessourceType, ERessourceSubType RessourceSubType, bool bRaw);

protected:
	virtual void BeginPlay() override;

	TArray<FInventoryItem> InventoryItems;

private:	
	

		
};
