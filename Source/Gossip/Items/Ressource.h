// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Gossip/AI/GS_AIController.h"
#include "Ressource.generated.h"

// TODO Refactor, this should be a SubClass of ARessource
UCLASS()
class GOSSIP_API ARessource : public AActor
{
	GENERATED_BODY()
	
public:	
	ARessource();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	EAIGoal RessourceType;

	UPROPERTY(EditAnywhere)
	FString CollectAction; //TODO Change to be a UAnimMontage

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	float CurrentDistanceToQuerier;

protected:
	virtual void BeginPlay() override;

private:
	void CollectRessource(class UInventoryComponent* InventoryComp);


};
