// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "DataLibrary.generated.h"

UENUM(BlueprintType)
enum class ECultures : uint8
{
	None			UMETA(DisplayName = "None"),
	Africa			UMETA(DisplayName = "Africa"),
	Caucase			UMETA(DisplayName = "Caucase"),
	Asia			UMETA(DisplayName = "Asia"),
	Arabic			UMETA(DisplayName = "Arabic"),
};

UENUM(BlueprintType)
enum class ECharacterProfile : uint8
{
	None			UMETA(DisplayName = "None"),
	Male			UMETA(DisplayName = "Male"),
	Female			UMETA(DisplayName = "Female"),
};

UENUM(BlueprintType)
enum class EAIStatus : uint8
{
	None				UMETA(DisplayName = "None"),
	PlayerOrder			UMETA(DisplayName = "PlayerOrder"),
	SearchSocialize		UMETA(DisplayName = "SearchSocialize"),
	Socialize			UMETA(DisplayName = "Socialize"),
	Follow				UMETA(DisplayName = "Follow"),
	LeadHome			UMETA(DisplayName = "LeadHome"),
	CityHallCall		UMETA(DisplayName = "CityHallCall"),
	Altruism			UMETA(DisplayName = "Altruism"),
};

// Instincts
UENUM(BlueprintType)
enum class EAIGoal : uint8
{
	None				UMETA(DisplayName = "None"),
	Food				UMETA(DisplayName = "Food"),
	Jerk				UMETA(DisplayName = "Jerk"),
	Leadership			UMETA(DisplayName = "Leadership"),
	Knowledge			UMETA(DisplayName = "Knowledge"),

	Rest				UMETA(DisplayName = "Rest"),
	Sleep				UMETA(DisplayName = "Sleep"),
	Shelter				UMETA(DisplayName = "Shelter"),
	Wealth				UMETA(DisplayName = "Wealth"),

	Sex					UMETA(DisplayName = "Sex"),
	Children			UMETA(DisplayName = "Children"),
	HandWork			UMETA(DisplayName = "HandWork"),
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
	Improve				UMETA(DisplayName = "Improve"),
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
	float ReportedValue;
	UPROPERTY(EditAnywhere)
	bool bRawStockable;
	UPROPERTY(EditAnywhere)
	bool bProcessedStockable;

};

// Social
USTRUCT(BlueprintType)
struct FAlignment
{
	GENERATED_BODY()

		UPROPERTY()
		float Respect;
	UPROPERTY()
		float Love;
	UPROPERTY()
		int32 Proximity;
	UPROPERTY()
		ECharacterProfile Gender;
};

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

//CityHall
UENUM(BlueprintType)
enum class ECityHallEvents : uint8
{
	None			UMETA(DisplayName = "None"),
	Alarm			UMETA(DisplayName = "Alarm"),
	Wedding			UMETA(DisplayName = "Wedding"),
	Banquet			UMETA(DisplayName = "Wedding"),
};

USTRUCT(BlueprintType)
struct FSavedCityHallEvent
{
	GENERATED_BODY()

	UPROPERTY()
	ECityHallEvents CityEvent;
	UPROPERTY()
	float TimeRemaining;
};

// Shelter
USTRUCT(BlueprintType)
struct FShelterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> WallMeshesPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> PillarMeshesPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> ConstructionMeshPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath ConstructionMontagePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ConstructionTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ShelterLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InventoryCapability;
};

// Ressource
UENUM(BlueprintType)
enum class ERessourceSubType : uint8
{
	None				UMETA(DisplayName = "None"),
	Vegetable			UMETA(DisplayName = "Vegetable"),
	Meat				UMETA(DisplayName = "Meat"),
	Wood				UMETA(DisplayName = "Wood"),
	Stone				UMETA(DisplayName = "Stone"),
	Iron				UMETA(DisplayName = "Iron"),
	SimpleBed			UMETA(DisplayName = "Simple Bed"),
	SimpleFire			UMETA(DisplayName = "Simple Fire"),
	SimpleRest			UMETA(DisplayName = "Simple Rest"),
};

USTRUCT(BlueprintType)
struct FRessourceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIGoal RessourceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSoftObjectPath> MeshesPtr;
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
	float RespawnTimeInGameHour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSoftObjectPath MontagePath;
};

// Characters
USTRUCT(BlueprintType)
struct FCharactersData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USkeletalMesh> MeshPtr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimBlueprintGeneratedClass* AnimBPClass;
};

USTRUCT(BlueprintType)
struct FCharacterName : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FirstName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LastName;
};

// Inventory
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	UPROPERTY()
	EAIGoal Ressource;
	UPROPERTY()
	ERessourceSubType RessourceSubType;
	UPROPERTY()
	bool bRaw;
	UPROPERTY()
	float Time;
};

// Sociology
UENUM(BlueprintType)
enum class EFamilySystem : uint8
{
	None			UMETA(DisplayName = "None"),
	Patriarcal		UMETA(DisplayName = "Patriarcal"),
	Matriarcal		UMETA(DisplayName = "Matriarcal"),
	Free			UMETA(DisplayName = "Free"),
};

UENUM(BlueprintType)
enum class EWeddingSystem : uint8
{
	None			UMETA(DisplayName = "None"),
	Monogamy		UMETA(DisplayName = "Monogamy"),
	Polygamy		UMETA(DisplayName = "Polygamy"),
	Free			UMETA(DisplayName = "Free"),
};

USTRUCT(BlueprintType)
struct FWeddingRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EFamilySystem FamilySystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeddingSystem WeddingSystem;
};

// SaveGame
USTRUCT(BlueprintType)
struct FSaveValues
{
	GENERATED_BODY()

	UPROPERTY()
	FString ComponentClassName;
	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	ECharacterProfile CharacterProfile;
	UPROPERTY()
	FCharacterName CharacterName;
	UPROPERTY()
	TMap<FGuid, FAlignment> KnownOthers;
	UPROPERTY()
	TArray<FInstinctValues> InstinctsValues;
	UPROPERTY()
	EAIGoal RessourceType;
	UPROPERTY()
	TArray<FGuid>Guids;
	UPROPERTY()
	int32 ContentCount;
	UPROPERTY()
	int32 DiversityIndex;
	UPROPERTY()
	TArray<FInventoryItem>Inventory;
	UPROPERTY()
	int32 ShelterLevel;
	UPROPERTY()
	float ShelterConstructionStep;
	UPROPERTY()
	TArray<int32> ShelterRemovedWalls;
	UPROPERTY()
	float CoolDown;
	UPROPERTY()
	TMap<FGuid, float>StoredWorkers;
	UPROPERTY()
	bool Boolean;
	UPROPERTY()
	FWeddingRule WeddingRules;
	UPROPERTY()
	FGuid FianceeGuid;
	UPROPERTY()
	TMap<FGuid, FSavedCityHallEvent> CityHallEvents;
	UPROPERTY()
	TMap<FGuid, FSavedCityHallEvent>OngoingCityEvent;
};

USTRUCT(BlueprintType)
struct FSaveStruct
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid Id;
	UPROPERTY()
	TMap<FString, FSaveValues> SaveValues;

};

UCLASS()
class GOSSIP_API UDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
