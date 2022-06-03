// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceCollector.h"
#include "InventoryComponent.h"

ARessourceCollector::ARessourceCollector()
{

}

void ARessourceCollector::CollectRessource(UInventoryComponent* InventoryComp)
{
	Super::CollectRessource(InventoryComp);
	if (ContentCount <= 0) return;

	ContentCount += -1;
	if (ContentCount <= 0) RessourceEmpty();

	InventoryComp->AddOwnedItem(RessourceType, true);
}

void ARessourceCollector::AddRessourceAsKnown(UInventoryComponent* InventoryComp)
{
	InventoryComp->AddKnownRessourceCollector(this);
}

void ARessourceCollector::BeginPlay()
{
	Super::BeginPlay();
}
