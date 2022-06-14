// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventory::BeginPlay()
{
	Super::BeginPlay();
	
}


void UInventory::AddOwnedItem(EAIGoal RessourceType, bool bRaw)
{
	FInventoryItem Item;
	Item.Ressource = RessourceType;
	Item.bRaw = bRaw;
	Item.Time = 0;
	InventoryItems.Add(Item);
}

void UInventory::RemoveOwnedItem(EAIGoal RessourceType, bool bRaw)
{
	int32 Index = 0;
	for (FInventoryItem Item : InventoryItems)
	{
		if (Item.Ressource == RessourceType && Item.bRaw == bRaw)
		{
			InventoryItems.RemoveAt(Index);
			return;
		}
		Index++;
	}
}

int32 UInventory::GetOwnedItemsCount(EAIGoal RessourceType, bool bRaw)
{
	// TODO check for Time and remove if spoiled
	int32 Count = 0;
	for (FInventoryItem Item : InventoryItems)
	{
		if (Item.Ressource == RessourceType && Item.bRaw == bRaw) Count++;
	}
	return Count;
}

