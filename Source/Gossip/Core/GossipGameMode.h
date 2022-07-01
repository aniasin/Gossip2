// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GossipGameMode.generated.h"


UCLASS(minimalapi)
class AGossipGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGossipGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GameHourDurationSeconds = 60; // for test purpose, should be 300 (aka 5 RealMinustes/1 GameHour)
	float CumulatedRealGameTime;

protected:

private:

};



