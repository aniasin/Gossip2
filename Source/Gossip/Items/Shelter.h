// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shelter.generated.h"

UCLASS()
class GOSSIP_API AShelter : public AActor
{
	GENERATED_BODY()
	
public:	
	AShelter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
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


protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)override;
#endif WITH_EDITOR

private:	


};
