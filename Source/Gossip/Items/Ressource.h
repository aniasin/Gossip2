// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Data/DataLibrary.h"
#include "GameFramework/Actor.h"

#include "Ressource.generated.h"



UCLASS()
class GOSSIP_API ARessource : public AActor
{
	GENERATED_BODY()
	
public:	
	ARessource();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class URessourceDataAsset* RessourceData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAIGoal RessourceType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AActor*> Owners;

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

	virtual void CollectRessource(class UInventoryComponent* InventoryComp);
	virtual void AddRessourceAsKnown(class UInventoryComponent* InventoryComp);

	void RessourceEmpty();
	void RessourceRespawn();

	float CurrentDistanceToQuerier;

protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR

	virtual void BeginPlay() override;	

private:
	class UMaterialInstanceDynamic* MaterialInstance;

};
