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

public:
	UPROPERTY()
	FString SaveGameName;
	UPROPERTY()
	FDateTime CreationTime;
	UPROPERTY()
	TMap<FGuid, FSaveStruct> SaveData;

	void CreateSaveGame(const FString& SlotName)
	{
		SaveGameName = SlotName;
		CreationTime = FDateTime::Now();
		SaveData.Empty();
		
	}
};
