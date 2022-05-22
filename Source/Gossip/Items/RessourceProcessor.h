// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Gossip/AI/GS_AIController.h"
#include "RessourceProcessor.generated.h"

UCLASS()
class GOSSIP_API ARessourceProcessor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARessourceProcessor();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere)
	EAIGoal RessourceType;
	UPROPERTY(EditAnywhere)
	FString ProcessAction; //TODO Change to be a UAnimMontage

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;

private:

	void ProcessRessource(class UInventoryComponent* InventoryComp);

};
