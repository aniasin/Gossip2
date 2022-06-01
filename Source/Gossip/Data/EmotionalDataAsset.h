// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataLibrary.h"
#include "EmotionalDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UEmotionalDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FEmotionalChangeTable>EmotionalChangeTable;
};
