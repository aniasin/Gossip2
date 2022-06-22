// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Data/DataLibrary.h"
#include "Gossip/Save/SaveGameInterface.h"
#include "SocialComponent.generated.h"


UCLASS( ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent) )
class GOSSIP_API USocialComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	
	USocialComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocialPosition SocialPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterProfile CharacterProfile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionalState OwnEmotionalState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data") 
	TArray<FSocialChangeTable> SocialChangeTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<FEmotionalChangeTable> EmotionalChangeTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TArray<FEmotionalUpdateTable> EmotionalUpdateTable;

	FGuid Id;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 ProximityScoreForFiancee = 10;

	EAlignmentState RefreshKnownOthers(AActor* Other);
	int32 InitiateInteraction(AActor* Other);
	int32 RespondToInteraction(AActor* Other);
	void EndInteraction(AActor* Other);

	void UpdateEmotionalState(TArray<EAIGoal>HungryInstincts);

	EAlignmentState GetAlignment(float Respect, float Love);

	AActor* FindSocialPartner();
	FVector FindFriendShelter();

	TMap<FGuid, FAlignment> GetKnownOthers() { return KnownOthers; }
	TArray<AActor*>GetFriends() { return Friends; }
	TArray<FGuid>GetFriendsGuid() { return FriendsGuid; }
	UFUNCTION(BlueprintCallable)
	EAlignmentState GetAlignmentState() {return CurrentAlignmentState;}
	UFUNCTION(BlueprintCallable)
	EEmotionalState GetEmotionalState() { return OwnEmotionalState; }

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;


protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	class USocialDataAsset* SocialChangeChartDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	class UEmotionalDataAsset* EmotionalChangeChartDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	class UEmotionalUpdateDataAsset* EmotionalUpdtateChartDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	ESocialPosition SocialPositionLike;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	ESocialPosition SocialPositionHate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	EEmotionalState EmotionalStateLike;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Taste")
	EEmotionalState EmotionalStateHate;

private:
	TMap<FGuid, FAlignment> KnownOthers;
	EAlignmentState CurrentAlignmentState;

	TArray<AActor*>Friends;
	TArray<FGuid>FriendsGuid;

	void UpdateAlignment(AActor* Other);
	FAlignment CalculateAlignmentChange(AActor* Other);
	void UpdateFriendList(AActor* Actor, AActor* Other, int32 Proximity);

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
