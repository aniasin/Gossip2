// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SocialComponent.generated.h"


UENUM(BlueprintType)
enum class EEmotionalState : uint8
{
	None		UMETA(DisplayName = "None"),
	Happy		UMETA(DisplayName = "Happy"),
	Sad			UMETA(DisplayName = "Sad"),
	Tragic		UMETA(DisplayName = "Tragic"),
	Energic		UMETA(DisplayName = "Energic"),
	Tired		UMETA(DisplayName = "Tired"),
	Confident	UMETA(DisplayName = "Confident"),
	Affraid		UMETA(DisplayName = "Affraid"),
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
struct FAlignment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Respect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Love;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API USocialComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USocialComponent();

	UPROPERTY(EditAnywhere)
	FAlignment Alignment_TEST;

	void NewActorInVincinity(AActor* Other);
	EAlignmentState GetAlignment(float Respect, float Love);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocialPosition SocialPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	ESocialPosition SocialPositionLike;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	ESocialPosition SocialPositionHate;

private:
	TMap<FString, FAlignment> KnownOther;
	EEmotionalState EmotionalState;

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
