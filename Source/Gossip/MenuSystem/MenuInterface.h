// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GOSSIP_API IMenuInterface
{
	GENERATED_BODY()
	
public:
	virtual void NewGame() = 0;
	virtual void LoadGame(FString SaveName) = 0;
	virtual void LoadMainMenu() = 0;
	virtual void QuitGame() = 0;
	virtual void SaveGame(FString SaveName) = 0;

};
