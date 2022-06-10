// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveableEntity.h"

USaveableEntity::USaveableEntity()
{
	PrimaryComponentTick.bCanEverTick = false;


}


void USaveableEntity::BeginPlay()
{
	Super::BeginPlay();


	
}

TMap<FString, FSaveValues> USaveableEntity::CaptureState(TMap<FString, FSaveValues>SaveData)
{
	TArray<UActorComponent*> Saveables = GetOwner()->GetComponentsByInterface(USaveGameInterface::StaticClass());
	for (UActorComponent* Saveable : Saveables)
	{
		ISaveGameInterface* SaveGameInterface = Cast<ISaveGameInterface>(Saveable);
		SaveGameInterface->CaptureState(SaveData);
	}

	return SaveData;
}

void USaveableEntity::RestoreState(TMap<FString, FSaveValues>SaveData)
{

}

