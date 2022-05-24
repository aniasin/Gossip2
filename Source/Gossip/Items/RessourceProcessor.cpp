// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceProcessor.h"
#include "InventoryComponent.h"


void ARessourceProcessor::CollectRessource(UInventoryComponent* InventoryComp)
{
	Super::CollectRessource(InventoryComp);
	if (InventoryComp)
	{
		InventoryComp->RemoveOwnedItem(RessourceType, true);
		InventoryComp->AddOwnedItem(RessourceType, false);
	}
}
