// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataLibrary.h"
#include "RessourceDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API URessourceDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EAIGoal, FRessourceData> RessourceDataMap;
};
