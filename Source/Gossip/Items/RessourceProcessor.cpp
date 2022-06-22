// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceProcessor.h"
#include "InventoryComponent.h"


ARessourceProcessor::ARessourceProcessor()
{

}

#if WITH_EDITOR
void ARessourceProcessor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif WITH_EDITOR

void ARessourceProcessor::CollectRessource(UInventoryComponent* InventoryComp, AActor* Actor)
{
	Super::CollectRessource(InventoryComp, Actor);

	if (InventoryComp)
	{
		InventoryComp->RemoveOwnedItem(RessourceType, RessourceSubType, true);
		InventoryComp->AddOwnedItem(RessourceType, RessourceSubType, false);
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
