// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
class GOSSIP_API ACityHall : public AActor
{
	GENERATED_BODY()
	
public:	
	ACityHall();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GameHoursWaitForWeddings = 12;

	void AddInhabitants(class ANonPlayerCharacter* Actors);

protected:
	virtual void BeginPlay() override;

private:
	TArray<ANonPlayerCharacter*>Inhabitants;

	TMap<FTimerHandle, FCityHallEvent>EventsQueue;

	UFUNCTION()
	void NewCityEvent(ECityHallEvents Event, TArray<AActor*>Guests);
	void BeginCityHallEvent();
	void ConvokeCityHallEvent(ECityHallEvents Event, TArray<AActor*>Guests);
};
