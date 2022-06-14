// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory.h"
#include "Gossip/Data/DataLibrary.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "InventoryComponent.generated.h"

class ARessource;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInventoryComponent : public UInventory, public ISaveGameInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY()
	AActor* ShelterActor;

	UFUNCTION()
	TArray<ARessource*> SortRessourcesByDistance(TArray<ARessource*> RessourceToSort);

	void AddKnownRessourceCollector(ARessource* RessourceActor);
	void RemoveKnownRessourceCollector(ARessource* RessourceActor);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceCollector(EAIGoal RessourceType);

	void AddKnownRessourceProcessor(ARessource* RessourceActor);
	// Warning can return nullptr
	AActor* SearchNearestKnownRessourceProcessor(EAIGoal RessourceType);

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

protected:
	virtual void BeginPlay()override;

private:

	TArray<ARessource*> KnownRessourcesCollector;
	TArray<ARessource*> KnownRessourcesProcessor;


	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
