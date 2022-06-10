// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveGameInterface.h"
#include "SaveableEntity.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API USaveableEntity : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USaveableEntity();

	FGuid Id;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	FSaveStruct CaptureState(FSaveStruct SaveData);
	void RestoreState(FSaveStruct SaveData);

		
};
