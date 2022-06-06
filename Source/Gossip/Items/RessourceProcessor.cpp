// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceProcessor.h"
#include "InventoryComponent.h"


ARessourceProcessor::ARessourceProcessor()
{

}

void ARessourceProcessor::CollectRessource(UInventoryComponent* InventoryComp)
{
	Super::CollectRessource(InventoryComp);

	if (InventoryComp)
	{
		InventoryComp->RemoveOwnedItem(RessourceType, true);
		InventoryComp->AddOwnedItem(RessourceType, false);
	}
}

void ARessourceProcessor::AddRessourceAsKnown(UInventoryComponent* InventoryComp)
{
	InventoryComp->AddKnownRessourceProcessor(this);
}

void ARessourceProcessor::BeginPlay()
{
	Super::BeginPlay();

}
