// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "InstinctsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInstinctsUpdateSignature, TArray<EAIGoal>, HungryInstincts);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInstinctsComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	
	UInstinctsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInstinctValues> InstinctsInfo;

	UPROPERTY()
	FOnInstinctsUpdateSignature OnInstinctsUpdated;

	TArray<EAIGoal>NotFoundRessources;

	void SatisfyInstinct(EAIGoal Goal);
	void TransferInstinctValue(EAIGoal GoalToTransfer, EAIGoal Destination);

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;


protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR

	virtual void BeginPlay() override;

	// Base Value for each
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UInstinctsDataAsset* InstinctDataAsset;

private:
	void InstinctsUpdate();
	void SortInstinctsByPriority();

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
