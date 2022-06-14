// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Gossip/Data/DataLibrary.h"
#include "ShelterDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UShelterDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<ESocialPosition, FShelterData> ShelterDataMap;
};
