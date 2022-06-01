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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocialPosition SocialPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionalState OwnEmotionalState;

	EAlignmentState RefreshKnownOthers(AActor* Other);
	bool InitiateInteraction(AActor* Other);
	bool RespondToInteraction(USocialComponent* OtherSocialComp);
	void EndInteraction(AActor* Other);

	void UpdateAlignment(AActor* Other, EAlignmentState EOwnAlignment, EAlignmentState OtherAlignment);

	EAlignmentState GetAlignment(float Respect, float Love);

	AActor* FindSocialPartner();

	TMap<FString, FAlignment> GetKnownOthers() { return KnownOthers; }
	UFUNCTION(BlueprintCallable)
	EAlignmentState GetAlignmentState() {return CurrentAlignmentState;}
	UFUNCTION(BlueprintCallable)
	EEmotionalState GetEmotionalState() { return OwnEmotionalState; }


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	ESocialPosition SocialPositionLike;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	ESocialPosition SocialPositionHate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	EEmotionalState EmotionalStateLike;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	EEmotionalState EmotionalStateHate;

private:
	TMap<FString, FAlignment> KnownOthers;
	EAlignmentState CurrentAlignmentState;

	float CalculateRespectChange(EAlignmentState OwnAlignment, EAlignmentState OtherAlignment, ESocialPosition OtherSocialPosition);
	float CompareSocialPositionTaste(ESocialPosition OtherSocialPosition);
	float GetRespectChange(EAlignmentState OwnAlignment, EAlignmentState OtherAlignment);
	float CalculateLoveChange(EEmotionalState CurrentEmotionalState, EEmotionalState OtherEmotionalState);
	float CompareEmotionalStateTaste(EEmotionalState OtherEmotionalState);
	float GetLoveChange(EEmotionalState OtherEmotionalState);

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
