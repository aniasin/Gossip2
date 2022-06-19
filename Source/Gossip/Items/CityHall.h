// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gossip/Save/SaveGameInterface.h"
#include "CityHall.generated.h"

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

	UFUNCTION()
	void NewCityEvent(ECityHallEvents Event, TArray<AActor*>Guests);
	// OverrideTime -1: no Override / 0 = Immediate / +1: Override
	void BeginCityHallEvent(float OverrideTime);
	UFUNCTION()
	void ConvokeCityHallEvent(FCityHallEvent Event);
};
