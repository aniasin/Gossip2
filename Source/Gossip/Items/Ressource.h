// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "GameFramework/Actor.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "Ressource.generated.h"



UCLASS()
class GOSSIP_API ARessource : public AActor, public ISaveGameInterface
{
	GENERATED_BODY()
	
public:	
	ARessource();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USaveableEntity* SaveGameComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class URessourceDataAsset* RessourceDataAsset;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERessourceSubType RessourceSubType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAIGoal RessourceType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 DiversityIndex;
	UPROPERTY()
	TArray<AActor*>Owners;

	UPROPERTY()
	bool bRaw;
	UPROPERTY()
	float WaitTime = 5;
	UPROPERTY()
	int32 ContentCount = 5;
	UPROPERTY()
	FLinearColor LivingColor;
	UPROPERTY()
	FLinearColor DeadColor;
	UPROPERTY()
	float RespawnTime;
	UPROPERTY()
	class UAnimMontage* AnimMontage; 

	virtual void CollectRessource(class UInventoryComponent* InventoryComp, AActor* Actor);
	virtual void AddRessourceAsKnown(class UInventoryComponent* InventoryComp);

	void RessourceEmpty();
	void RessourceRespawn();
	float StartWorking(AActor* Actor);
	void StopWorking(AActor* Actor);
	int32 GetRessourceDisponibility();

	class UAnimMontage* GetAnimMontageMontage();

	float CurrentDistanceToQuerier;

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR

	virtual void BeginPlay() override;	

private:
	FRessourceData RessourceData;
	class UMaterialInstanceDynamic* MaterialInstance;

	int32 MaxContentCount;
	FTimerHandle TimerRespawn;

	TArray<AActor*>ActorsWorkingOn;
};
