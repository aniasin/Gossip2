// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CityHall.generated.h"

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

	void AddInhabitants(class ANonPlayerCharacter* Actors);

protected:
	virtual void BeginPlay() override;

private:
	TArray<ANonPlayerCharacter*>Inhabitants;

	UFUNCTION()
	void NewCityEvent();
};
