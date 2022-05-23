// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Ressource.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::AddKnownRessourceCollector(ARessource* RessourceActor)
{
	KnownRessourcesCollector.AddUnique(RessourceActor);
}

int32 UInventoryComponent::GetKnownRessourcesCount(EAIGoal RessourceType)
{
	int32 Index = 0;
	int32 Count = 0;
	for (ARessource* Ressource : KnownRessourcesCollector)
	{
		if (!Ressource) { KnownRessourcesCollector.RemoveAt(Index);	break; }
		if (Ressource->RessourceType == RessourceType)
		{
			Count++;
		}
	}
	return Count;
}

AActor* UInventoryComponent::SearchNearestKnownRessource(EAIGoal RessourceType)
{
	int32 Index = 0;
	TArray<ARessource*>RessourcesToSort;
	for (ARessource* Ressource : KnownRessourcesCollector)
	{
		if (!Ressource) { KnownRessourcesCollector.RemoveAt(Index);	break; }
		if (Ressource->RessourceType == RessourceType)
		{
			RessourcesToSort.Add(Ressource);
		}
	}
	for (ARessource* Ressource : RessourcesToSort)
	{
		float Distance = FVector::Distance(Ressource->GetActorLocation(), GetOwner()->GetActorLocation());
		Ressource->CurrentDistanceToQuerier = Distance;
	}
	TArray<ARessource*>SortedRessources = SortRessourcesByDistance(RessourcesToSort);
	
	return SortedRessources[0];
}

void UInventoryComponent::AddOwnedItem(EAIGoal RessourceType, bool bRaw)
{
	FInventoryItem Item;
	Item.Ressource = RessourceType;
	Item.bRaw = bRaw;
	Item.Time = 0;
	InventoryItems.Add(Item);
}

int32 UInventoryComponent::GetOwnedItemsCount(EAIGoal RessourceType, bool bRaw)
{
	// TODO check for Time and remove if spoiled
	int32 Count = 0;
	for (FInventoryItem Item : InventoryItems)
	{
		if (Item.Ressource == RessourceType && Item.bRaw == bRaw) Count++;
	}
	return Count;
}

TArray<ARessource*> UInventoryComponent::SortRessourcesByDistance(TArray<ARessource*> RessourceToSort)
{
	auto SortPred = [](ARessource& A, ARessource& B)->bool
	{
		return(A.CurrentDistanceToQuerier) <= (B.CurrentDistanceToQuerier);
	};
	RessourceToSort.Sort(SortPred);

	return RessourceToSort;
}

