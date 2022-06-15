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

	// Warning! This must be regenerated when Alt-Moving (duplicating) actors!
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

	UPROPERTY(VisibleAnywhere)
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
	UStaticMeshComponent* ConstructionMesh;

	UPROPERTY(VisibleAnywhere)
	class UInventory* Inventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UShelterDataAsset* ShelterDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ANonPlayerCharacter> NpcToSpawnClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ARessource* SleepCollector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ARessource* FoodProcessor;
	UPROPERTY(EditAnywhere)
	ESocialPosition ShelterGrade;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* ConstructMontage;
	UPROPERTY()
	TArray<AActor*>Owners;
	UPROPERTY()
	int32 CurrentLevel;
	UPROPERTY()
	int32 ConstructionStep;
	UPROPERTY()
	float ConstructionTime;
	UPROPERTY()
	float CurrentConstructionStep;

	void BeginConstruct();
	void ConstructShelter();


protected:
	virtual void BeginPlay() override;

	void SpawnNPC();

	void SetShelterSize();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR

	void InitializeShelter();

private:	
	void UpgradeShelter();

	void OnAsyncLoadComplete();
	void OnAsyncLoadConstructionComplete();

	FShelterData ShelterData;

};
