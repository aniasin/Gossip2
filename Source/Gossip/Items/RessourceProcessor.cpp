// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceProcessor.h"
#include "InventoryComponent.h"


void ARessourceProcessor::CollectRessource(UInventoryComponent* InventoryComp)
{
	Super::CollectRessource(InventoryComp);
	//TODO actual Processing Action and Delay
	if (InventoryComp)
	{
		InventoryComp->AddOwnedItem(RessourceType, false);
	}
}
