// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Ressource.h"
#include "Shelter.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	AShelter* OwnedShelter = Cast<AShelter>(ShelterActor);
	if (OwnedShelter) OwnedShelter->Owners.AddUnique(GetOwner());
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

TArray<ARessource*> UInventoryComponent::SortRessourcesByDistance(TArray<ARessource*> RessourceToSort)
{
	auto SortPred = [](ARessource& A, ARessource& B)->bool
	{
		return(A.CurrentDistanceToQuerier) < (B.CurrentDistanceToQuerier);
	};
	RessourceToSort.Sort(SortPred);

	return RessourceToSort;
}

// ISavegameInterface override
FSaveValues UInventoryComponent::CaptureState()
{
	FSaveValues SaveValues;
	SaveValues.ComponentClassName = GetClass()->GetName();
	SaveValues.Inventory = InventoryItems;
	
	return SaveValues;
}

void UInventoryComponent::RestoreState(FSaveValues SaveValues)
{
	InventoryItems = SaveValues.Inventory;
}

