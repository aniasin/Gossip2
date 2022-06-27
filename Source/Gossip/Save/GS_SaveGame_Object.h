// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "Gossip/Data/DataLibrary.h"
#include "GS_SaveGame_Object.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UGS_SaveGame_Object : public USaveGame
{
	GENERATED_BODY()
	
	UGS_SaveGame_Object();

public:
	UPROPERTY()
	FString SaveGameName;
	UPROPERTY()
	FString MapName;
	UPROPERTY()
	FDateTime CreationTime;
	UPROPERTY()
	float GameTimeSeconds;
	UPROPERTY()
	TMap<FGuid, FSaveStruct> SaveData;

};
