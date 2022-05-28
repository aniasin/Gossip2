// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataLibrary.generated.h"

UENUM(BlueprintType)
enum class EAIGoal : uint8
{
	None				UMETA(DisplayName = "None"),
	Food				UMETA(DisplayName = "Food"),
	Leadership			UMETA(DisplayName = "Leadership"),
	Knowledge			UMETA(DisplayName = "Knowledge"),

	Sleep				UMETA(DisplayName = "Sleep"),
	Shelter				UMETA(DisplayName = "Shelter"),
	Wealth				UMETA(DisplayName = "Wealth"),

	Sex					UMETA(DisplayName = "Sex"),
	Children			UMETA(DisplayName = "Children"),
	Work				UMETA(DisplayName = "Work"),
};

UENUM(BlueprintType)
enum class EAIAction : uint8
{
	None				UMETA(DisplayName = "None"),
	SearchCollector		UMETA(DisplayName = "SearchCollector"),
	StockRaw			UMETA(DisplayName = "StockRaw"),
	StockProcessed		UMETA(DisplayName = "StockProcessed"),
	Satisfy				UMETA(DisplayName = "Satisfy"),
	SearchProcessor		UMETA(DisplayName = "SearchProcessor"),
	TravelCollector		UMETA(DisplayName = "TravelCollector"),
	TravelProcessor		UMETA(DisplayName = "TravelProcessor"),
};

UENUM(BlueprintType)
enum class EAIInstinct : uint8
{
	None				UMETA(DisplayName = "None"),
	Assimilation		UMETA(DisplayName = "Assimilation"),
	Conservation		UMETA(DisplayName = "Conservation"),
	Reproduction		UMETA(DisplayName = "Reproduction"),
};

USTRUCT(BlueprintType)
struct FInstinctValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EAIInstinct Instinct;
	UPROPERTY(EditAnywhere)
	EAIGoal Goal;
	UPROPERTY(EditAnywhere)
	float CurrentValue;
	UPROPERTY(EditAnywhere)
	float UpdateMultiplier;
	UPROPERTY(EditAnywhere)
	float GrowCoeffient;
	UPROPERTY(EditAnywhere)
	bool bStockable;
};


UCLASS()
class GOSSIP_API UDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
