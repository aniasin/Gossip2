// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::AddKnownRessource(ARessource* RessourceActor)
{
	KnownRessources.AddUnique(RessourceActor);
}

int32 UInventoryComponent::GetKnownRessourcesCount(EAIGoal RessourceType)
{
	int32 Count = 0;
	for (ARessource* Ressource : KnownRessources)
	{
		if (Ressource->RessourceType == RessourceType)
		{
			Count++;
		}
	}
	return Count;
}

