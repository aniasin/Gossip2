// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataLibrary.h"
#include "CharactersDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UCharactersDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<ECharacterProfile, FCharactersData>CharactersData;
	
};
