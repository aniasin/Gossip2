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

int32 UInventoryComponent::GetKnownRessourcesCollectorCount(EAIGoal RessourceType)
{
	int32 Index = 0;
	int32 Count = 0;
	ClearNullRessources(KnownRessourcesCollector);
	for (ARessource* Ressource : KnownRessourcesCollector)
	{
		if (Ressource->RessourceType == RessourceType)
		{
			Count++;
		}
	}
	return Count;
}

int32 UInventoryComponent::GetKnownRessourcesProcessorCount(EAIGoal RessourceType)
{
	int32 Index = 0;
	int32 Count = 0;
	ClearNullRessources(KnownRessourcesProcessor);
	for (ARessource* Ressource : KnownRessourcesProcessor)
	{
		if (Ressource->RessourceType == RessourceType)
		{
			Count++;
		}
	}
	return Count;
}

void UInventoryComponent::AddKnownRessourceProcessor(ARessource* RessourceActor)
{
	KnownRessourcesProcessor.AddUnique(RessourceActor);
	UE_LOG(LogTemp, Warning, TEXT("%s has been added to KnownRessources!"), *RessourceActor->GetName())
}

AActor* UInventoryComponent::SearchNearestKnownRessourceProcessor(EAIGoal RessourceType)
{
	int32 Index = 0;
	if (KnownRessourcesCollector.Num() <= 0) return nullptr;
	ClearNullRessources(KnownRessourcesProcessor);
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

	if (RessourcesToSort.Num() <= 0) return nullptr;
	return SortedRessources[0];
}

AActor* UInventoryComponent::SearchNearestKnownRessourceCollector(EAIGoal RessourceType)
{
	int32 Index = 0;
	if (KnownRessourcesCollector.Num() <= 0) return nullptr;
	ClearNullRessources(KnownRessourcesCollector);
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
	
	if (RessourcesToSort.Num() <= 0) return nullptr;
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

void UInventoryComponent::ClearNullRessources(TArray<ARessource*> Array)
{
	TArray<ARessource*>ArrayCleaned;
	for (ARessource* Ressource : Array)
	{
		if (Ressource && IsValid(Ressource)) { ArrayCleaned.AddUnique(Ressource); }
	}
	Array = ArrayCleaned;
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

