// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataLibrary.h"
#include "InstinctsDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UInstinctsDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Instincs")
	TArray<FInstinctValues> InstinctsInfo;
	
};
