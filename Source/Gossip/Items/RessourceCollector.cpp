// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceCollector.h"
#include "InventoryComponent.h"

ARessourceCollector::ARessourceCollector()
{

}

#if WITH_EDITOR
void ARessourceCollector::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif WITH_EDITOR

void ARessourceCollector::CollectRessource(UInventoryComponent* InventoryComp)
{
	if (!IsValid(InventoryComp)) return;

	Super::CollectRessource(InventoryComp);

	if (ContentCount <= 0) return;
	ContentCount += -1;
	if (ContentCount <= 0) RessourceEmpty();
	InventoryComp->AddOwnedItem(RessourceType, RessourceSubType, true);
}

void ARessourceCollector::AddRessourceAsKnown(UInventoryComponent* InventoryComp)
{
	if (!IsValid(InventoryComp)) return;

	InventoryComp->AddKnownRessourceCollector(this);
}

void ARessourceCollector::BeginPlay()
{
	Super::BeginPlay();
}
