// Copyright Epic Games, Inc. All Rights Reserved.

#include "GossipGameMode.h"
#include "GossipCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGossipGameMode::AGossipGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Pawn"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;

	}
}
