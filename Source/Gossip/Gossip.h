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
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAIGoal, EAIGoal::Food, EAIGoal::Sex);

UENUM(BlueprintType)
enum class EAIInstinct : uint8
{
	Assimilation				UMETA(DisplayName = "None"),
	Conservation				UMETA(DisplayName = "Food"),
	Reproduction				UMETA(DisplayName = "Sleep"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EAIInstinct, EAIInstinct::Assimilation, EAIInstinct::Reproduction);