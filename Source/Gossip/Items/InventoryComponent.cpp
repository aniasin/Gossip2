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

AActor* UInventoryComponent::SearchNearestKnownRessource(EAIGoal RessourceType)
{
	TArray<ARessource*>RessourcesToSort;
	for (ARessource* RessourceActor : KnownRessources)
	{
		ARessource* Ressource = Cast<ARessource>(RessourceActor);
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

TArray<ARessource*> UInventoryComponent::SortRessourcesByDistance(TArray<ARessource*> RessourceToSort)
{
	auto SortPred = [](ARessource& A, ARessource& B)->bool
	{
		return(A.CurrentDistanceToQuerier) <= (B.CurrentDistanceToQuerier);
	};
	RessourceToSort.Sort(SortPred);

	return RessourceToSort;
}

