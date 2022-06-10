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

FSaveStruct USaveableEntity::CaptureState(FSaveStruct SaveData)
{
	TArray<UActorComponent*> Saveables = GetOwner()->GetComponentsByInterface(USaveGameInterface::StaticClass());
	for (UActorComponent* Saveable : Saveables)
	{
		ISaveGameInterface* SaveGameInterface = Cast<ISaveGameInterface>(Saveable);
		FSaveValues Values = SaveGameInterface->CaptureState();

		SaveData.Id = Id;
		SaveData.SaveValues.Add(GetClass()->GetName(), Values);
	}

	return SaveData;
}

void USaveableEntity::RestoreState(FSaveStruct SaveData)
{

}

