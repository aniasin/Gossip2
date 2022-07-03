// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "Gossip/Data/DataLibrary.h"
#include "Shelter.generated.h"

UCLASS()
class GOSSIP_API AShelter : public AActor, public ISaveGameInterface
{
	GENERATED_BODY()
	
public:	
	AShelter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USaveableEntity* SaveComponent;

	// Warning! This must be regenerated when Alt-Moving (duplicating) actors!
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGuid OwnId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* PlayerRuler;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid SpawnedNpcId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 400, ClampMax = 600))
	float ShelterSizeX = 400;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 300, ClampMax = 600))
	float ShelterSizeY = 300;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Floor;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Ramp;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallFront;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallBack;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallRight;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WallLeft;
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
	class ACityHall* CityHall;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ARessource* SleepCollector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ARessource* FoodProcessor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ARessource* RestCollector;
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

	float BeginConstruct(AActor* Controller);
	void StopConstruct(AActor* Controller);

	float BeginHandwork(AActor* Controller);
	void StopHandwork(AActor* Controller);

	void LoadConstructionMeshes();

	void ConstructShelter(AActor* Controller);
	bool GetIsWorkingOn(AActor* Controller) { return ActorsWorkingOn.Contains(Controller); }

	ERessourceSubType GetRessourceSubTypeForImprovement() { return RessourceForImprovement; }

	//this is Called by GameInstance
	void SpawnNPC();
	void InitializeShelter();
	void MoveShelter(AShelter* NewShelter);
	void RemoveWall(int32 WallIndex);

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveValues)override;

protected:
	virtual void BeginPlay() override;



	void RegisterNpcToCityHall(ANonPlayerCharacter* NPC);

	void SetShelterSize();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR

private:	
	void UpgradeShelter();
	FVector TraceForSpaceInDirection(AShelter* NewShelter, FVector Direction);
	FTransform TraceForTerrainHeight(AShelter* NewShelter, FVector FreeLocation);

	void AddToOwners(AActor* Actor);

	void OnAsyncLoadComplete();
	void OnAsyncLoadConstructionComplete();

	FShelterData ShelterData;
	ERessourceSubType RessourceForImprovement;
	TArray<int32> ShelterRemovedWalls;
	class ARessource* CurrentImprovingFurniture;

	TArray<AActor*>ActorsWorkingOn;
	TArray<float>Timers;
	TMap<FGuid, float>StoredWorkers;
};
