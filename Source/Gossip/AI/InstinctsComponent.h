// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "Components/ActorComponent.h"

#include "InstinctsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInstinctsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstinctsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FInstinctValues> InstinctsInfo;

	void SatisfyInstinct(EAIGoal Goal);


protected:
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
