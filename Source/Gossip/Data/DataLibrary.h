// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataLibrary.generated.h"

UENUM(BlueprintType)
enum class EAIStatus : uint8
{
	None				UMETA(DisplayName = "None"),
	PlayerOrder			UMETA(DisplayName = "PlayerOrder"),
	SearchSocialize		UMETA(DisplayName = "SearchSocialize"),
	Socialize			UMETA(DisplayName = "Socialize"),
};

// Instincts
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

// Social
UENUM(BlueprintType)
enum class EEmotionalState : uint8
{
	None		UMETA(DisplayName = "None"),
	Happy		UMETA(DisplayName = "Happy"),
	Sad			UMETA(DisplayName = "Sad"),
	Idealist	UMETA(DisplayName = "Idealist"),
	Tragic		UMETA(DisplayName = "Tragic"),
	Energic		UMETA(DisplayName = "Energic"),
	Tired		UMETA(DisplayName = "Tired"),
	Confident	UMETA(DisplayName = "Confident"),
	Affraid		UMETA(DisplayName = "Affraid"),
	Cold		UMETA(DisplayName = "Cold"),
	Panicked    UMETA(DisplayName = "Panicked"),
};

UENUM(BlueprintType)
enum class EAlignmentState : uint8
{
	None			UMETA(DisplayName = "None"),
	Submissive		UMETA(DisplayName = "Submissive"),
	Cooperative     UMETA(DisplayName = "Cooperative"),
	Imperious		UMETA(DisplayName = "Imperious"),
	Masterful		UMETA(DisplayName = "Masterful"),
};

UENUM(BlueprintType)
enum class ESocialPosition : uint8
{
	None			UMETA(DisplayName = "None"),
	Noble			UMETA(DisplayName = "Noble"),
	Bourgeois		UMETA(DisplayName = "Bourgeois"),
	Worker			UMETA(DisplayName = "Worker"),
	Tchandala		UMETA(DisplayName = "Tchandala"),
};

USTRUCT(BlueprintType)
struct FSocialChangeTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EAlignmentState Alignment;
	UPROPERTY(EditAnywhere)
	TMap<EAlignmentState, float>OtherAlignmentEffect;
};

USTRUCT(BlueprintType)
struct FEmotionalChangeTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EEmotionalState EmotionalState;
	UPROPERTY(EditAnywhere)
		TMap<EEmotionalState, float>OtherEmotionalStateEffect;
};

USTRUCT(BlueprintType)
struct FEmotionalUpdateTable
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EAIGoal Instinct;
	UPROPERTY(EditAnywhere)
	TMap<EEmotionalState, float>EmotionalEffect;
};

// Ressource
USTRUCT(BlueprintType)
struct FRessourceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIGoal RessourceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath MeshSoftPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WaitTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ContentCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LivingColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor DeadColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RespawnTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage;
};

UCLASS()
class GOSSIP_API UDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
