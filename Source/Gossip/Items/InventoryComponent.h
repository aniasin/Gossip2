// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "Components/ActorComponent.h"

#include "InventoryComponent.generated.h"

class ARessource;

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	EAIGoal Ressource;
	UPROPERTY()
	bool bRaw;
	UPROPERTY()
	float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(VisibleAnywhere) // For testing, should be private
	TArray<FInventoryItem> InventoryItems;

	UFUNCTION(BlueprintCallable)
	TArray<ARessource*> SortRessourcesByDistance(TArray<ARessource*> RessourceToSort);

	void AddKnownRessourceCollector(ARessource* RessourceActor);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceCollector(EAIGoal RessourceType);

	void AddKnownRessourceProcessor(ARessource* RessourceActor);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceProcessor(EAIGoal RessourceType);

	void AddOwnedItem(EAIGoal RessourceType, bool bRaw);
	void RemoveOwnedItem(EAIGoal RessourceType, bool bRaw);
	int32 GetOwnedItemsCount(EAIGoal RessourceType, bool bRaw);

private:

	TArray<ARessource*> KnownRessourcesCollector;
	TArray<ARessource*> KnownRessourcesProcessor;
	TArray<ARessource*> ClearNullRessources(TArray<ARessource*> Array);

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
