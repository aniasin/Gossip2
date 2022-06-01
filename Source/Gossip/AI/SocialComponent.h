// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Data/DataLibrary.h"
#include "SocialComponent.generated.h"


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

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Data")
	TArray<FSocialChangeTable> SocialChangeTable;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Data")
	TArray<FEmotionalChangeTable> EmotionalChangeTable;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	class USocialDataAsset* SocialChangeChartDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	class UEmotionalDataAsset* EmotionalChangeChartDataAsset;

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
	TArray<FSocialChangeTable> SocialChangeChart;
	TArray<FEmotionalChangeTable> EmotionalChangeChart;

	TMap<FString, FAlignment> KnownOthers;
	EAlignmentState CurrentAlignmentState;

	float CalculateRespectChange(EAlignmentState OwnAlignment, EAlignmentState OtherAlignment, ESocialPosition OtherSocialPosition);
	float CalculateLoveChange(EEmotionalState CurrentEmotionalState, EEmotionalState OtherEmotionalState);

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
