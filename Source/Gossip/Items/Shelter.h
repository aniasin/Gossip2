// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Gossip/Data/DataLibrary.h"
#include "Shelter.generated.h"

UCLASS()
class GOSSIP_API AShelter : public AActor
{
	GENERATED_BODY()
	
public:	
	AShelter();

	UPROPERTY()
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallA;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallB;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallC;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallD;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PillarA;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PillarB;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PillarC;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PillarD;

	UPROPERTY(VisibleAnywhere)
	class UInventory* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UShelterDataAsset* ShelterDataAsset;
	UPROPERTY(EditAnywhere)
	ESocialPosition ShelterGrade;
	UPROPERTY()
	int32 ConstructionStep;
	UPROPERTY()
	float ConstructionTime;


protected:
	virtual void BeginPlay() override;

	void SetShelterSize();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;

	void InitializeShelter();

#endif WITH_EDITOR

private:	
	void OnAsyncLoadComplete();

	FShelterData ShelterData;

};
