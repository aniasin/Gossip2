// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "FamilyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNewCityHallEvent, ECityHallEvents, Event, TArray<AActor*>, GuestsGuid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLastNameChanged, FCharacterName, NewCharacterName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovingShelter, AActor*, OtherActor);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UFamilyComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	

	UFamilyComponent();

	FGuid Id;

	ECharacterProfile CharacterGender;
	FCharacterName CharacterName;

	FOnLastNameChanged OnLastNameChanged;
	FOnMovingShelter OnMovingShelter;

	void RequestWedding(AActor* Other, FWeddingRule WeddingRule);

	AActor* GetCurrentFiancee() { return CurrentFiancee; }
	TArray<AActor*>GetSpouses() { return Spouses; }

	void SetCurrentFiancee(AActor* Fiancee) { CurrentFiancee = Fiancee; }
	void ResetOwnersAI(AActor* Other);

	void CityHallCalling(FVector Location);
	void Marry();

	void SetWeddingRule(FWeddingRule Rule) { CurrentWeddingRules = Rule; }

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

	FOnNewCityHallEvent OnNewCityHallEvent;

protected:

	virtual void BeginPlay() override;

private:
	FTimerHandle ScheduledWeddingTimerHandle;
	FWeddingRule CurrentWeddingRules;
	TArray<AActor* > Spouses;
	AActor* CurrentFiancee;		

	TArray<AActor*> GetGuests(AActor* Other);
	void ChangeName(AActor* Actor);

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
	
};
