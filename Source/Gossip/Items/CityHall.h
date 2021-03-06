// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gossip/Save/SaveGameInterface.h"
#include "Gossip/Data/DataLibrary.h"
#include "CityHall.generated.h"

USTRUCT(BlueprintType)
struct FCityHallEvent
{
	GENERATED_BODY()

	UPROPERTY()
	ECityHallEvents CityEvent;
	UPROPERTY()
	TArray<AActor*>Guests;
};

UCLASS()
class GOSSIP_API ACityHall : public AActor, public ISaveGameInterface
{
	GENERATED_BODY()
	
public:	
	ACityHall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USaveableEntity* SaveComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CityEventGameHourDuration = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GameHoursWaitForEvent = 12;

	void AddInhabitants(class ANonPlayerCharacter* Actors);

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveValues)override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime)override;

private:
	TArray<ANonPlayerCharacter*>Inhabitants;
	FTimerHandle CityEventTimerHandle;
	TArray<FCityHallEvent>EventsQueued;
	FTimerHandle OngoingEventTimerHandle;

	UFUNCTION()
	void NewCityEvent(ECityHallEvents Event, TArray<AActor*>Guests);
	// OverrideTime -1: no Override / 0 = Immediate / +1: Override
	UFUNCTION()
	void BeginCityHallEvent(float OverrideTime);
	UFUNCTION()
	void ConvokeCityHallEvent(FCityHallEvent Event, float OverrideTime);
	UFUNCTION()
	void EndCityEvent(ECityHallEvents Event);

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
