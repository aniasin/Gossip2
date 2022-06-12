// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "InventoryComponent.generated.h"

class ARessource;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInventoryComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite) // For testing, should be private
	int32 StockingLimit = 10;

	UFUNCTION()
	TArray<ARessource*> SortRessourcesByDistance(TArray<ARessource*> RessourceToSort);

	void AddKnownRessourceCollector(ARessource* RessourceActor);
	void RemoveKnownRessourceCollector(ARessource* RessourceActor);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceCollector(EAIGoal RessourceType);

	void AddKnownRessourceProcessor(ARessource* RessourceActor);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceProcessor(EAIGoal RessourceType);

	void AddOwnedItem(EAIGoal RessourceType, bool bRaw);
	void RemoveOwnedItem(EAIGoal RessourceType, bool bRaw);
	int32 GetOwnedItemsCount(EAIGoal RessourceType, bool bRaw);

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

private:

	TArray<ARessource*> KnownRessourcesCollector;
	TArray<ARessource*> KnownRessourcesProcessor;
	TArray<FInventoryItem> InventoryItems;

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
