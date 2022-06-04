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

void UInventoryComponent::RemoveKnownRessourceCollector(ARessource* RessourceActor)
{
	KnownRessourcesCollector.Remove(RessourceActor);
}

void UInventoryComponent::AddKnownRessourceProcessor(ARessource* RessourceActor)
{
	KnownRessourcesProcessor.AddUnique(RessourceActor);
}

AActor* UInventoryComponent::SearchNearestKnownRessourceProcessor(EAIGoal RessourceType)
{
	if (KnownRessourcesProcessor.Num() <= 0) return nullptr;
	TArray<ARessource*>RessourcesToSort;
	for (ARessource* Ressource : KnownRessourcesProcessor)
	{
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

	if (SortedRessources.Num() <= 0) return nullptr;
	return SortedRessources[0];
}

AActor* UInventoryComponent::SearchNearestKnownRessourceCollector(EAIGoal RessourceType)
{
	if (KnownRessourcesCollector.Num() <= 0) return nullptr;
	TArray<ARessource*>RessourcesToSort;
	for (ARessource* Ressource : KnownRessourcesCollector)
	{
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

	if (SortedRessources.Num() <= 0) return nullptr;
	return SortedRessources[0];
}

void UInventoryComponent::AddOwnedItem(EAIGoal RessourceType, bool bRaw)
{
	FInventoryItem Item;
	Item.Ressource = RessourceType;
	Item.bRaw = bRaw;
	Item.Time = 0;
	InventoryItems.Add(Item);
	FString ItemString = GetEnumValueAsString<EAIGoal>("EAIGoal", RessourceType);
	FString RawString;
	bRaw ? RawString = "Raw" : RawString = "Processed";
	//UE_LOG(LogTemp, Warning, TEXT("%s %s has been added to inventory! New count: %i"), *RawString, *ItemString, GetOwnedItemsCount(RessourceType, bRaw))
}

void UInventoryComponent::RemoveOwnedItem(EAIGoal RessourceType, bool bRaw)
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
		return(A.CurrentDistanceToQuerier) < (B.CurrentDistanceToQuerier);
	};
	RessourceToSort.Sort(SortPred);

	return RessourceToSort;
}

