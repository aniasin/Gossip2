// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"
#include "GS_Singleton.generated.h"

/**
 * 
 */
UCLASS(Config=Game, notplaceable)
class GOSSIP_API UGS_Singleton : public UObject, public FTSTickerObjectBase
{
	GENERATED_BODY()

private:
	UGS_Singleton();

public:
	static UGS_Singleton& Get();
	FStreamableManager AssetLoader;

	virtual bool Tick(float DeltaTime) override;
	
};
