// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Items/Ressource.h"
#include "InventoryComponent.generated.h"

class ARessource;

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Item;
	UPROPERTY()
	float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	void AddKnownRessource(ARessource* RessourceActor);
	int32 GetKnownRessourcesCount(EAIGoal RessourceType);

private:
	TArray<FInventoryItem> InventoryItems;
	TArray<ARessource*> KnownRessources;
		

};
