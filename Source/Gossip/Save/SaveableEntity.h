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
	USaveableEntity();

	// This is set in the Owner Constructor BP
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;

protected:

public:	
	FSaveStruct CaptureState(FSaveStruct SaveData);
	void RestoreState(FSaveStruct SaveData);

		
};
