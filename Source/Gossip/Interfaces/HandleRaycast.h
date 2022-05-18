// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HandleRaycast.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHandleRaycast : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GOSSIP_API IHandleRaycast
{
	GENERATED_BODY()

public:
	virtual bool HandleRaycast(APlayerController* PlayerController) = 0;
	virtual void SetSelected(bool Value) = 0;
};
