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

FSaveStruct USaveableEntity::CaptureState(FSaveStruct SaveStruct)
{
	TArray<UActorComponent*> Saveables = GetOwner()->GetComponentsByInterface(USaveGameInterface::StaticClass());
	for (UActorComponent* Saveable : Saveables)
	{
		ISaveGameInterface* SaveGameInterface = Cast<ISaveGameInterface>(Saveable);
		FSaveValues Values = SaveGameInterface->CaptureState();

		SaveStruct.Id = Id;
		SaveStruct.SaveValues.Add(Saveable->GetClass()->GetName(), Values);
	}
	return SaveStruct;
}

void USaveableEntity::RestoreState(FSaveStruct SaveStruct)
{
	TArray<UActorComponent*> Saveables = GetOwner()->GetComponentsByInterface(USaveGameInterface::StaticClass());
	for (UActorComponent* Saveable : Saveables)
	{
		ISaveGameInterface* SaveGameInterface = Cast<ISaveGameInterface>(Saveable);
		for (auto& Values : SaveStruct.SaveValues)
		{
			if (SaveStruct.Id != Id) continue;
			if (Values.Key == Saveable->GetClass()->GetName()) SaveGameInterface->RestoreState(Values.Value);
		}
	}
}

