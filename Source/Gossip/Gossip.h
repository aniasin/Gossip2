// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAIGoal : uint8
{
	None				UMETA(DisplayName = "None"),
	Food				UMETA(DisplayName = "Food"),
	Sleep				UMETA(DisplayName = "Sleep"),
	Sex					UMETA(DisplayName = "Sex"),
	Knowledge			UMETA(DisplayName = "Knowledge"),
	Shelter				UMETA(DisplayName = "Shelter"),
	Children			UMETA(DisplayName = "Children"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAIGoal, EAIGoal::Food, EAIGoal::Children);

UENUM(BlueprintType)
enum class EAIInstinct : uint8
{
	None				UMETA(DisplayName = "None"),
	Assimilation		UMETA(DisplayName = "Assimilation"),
	Conservation		UMETA(DisplayName = "Conservation"),
	Reproduction		UMETA(DisplayName = "Reproduction"),
};
