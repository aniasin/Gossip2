// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveableEntity.h"

USaveableEntity::USaveableEntity()
{
	PrimaryComponentTick.bCanEverTick = false;

}

FSaveStruct USaveableEntity::CaptureState(FSaveStruct SaveStruct)
{
	if (ISaveGameInterface* SaveGameInterface = Cast<ISaveGameInterface>(GetOwner()))
	{
		FSaveValues Values = SaveGameInterface->CaptureState();

		SaveStruct.Id = Id;
		SaveStruct.SaveValues.Add(GetOwner()->GetClass()->GetName(), Values);
	}

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
	if (ISaveGameInterface* SaveGameInterface = Cast<ISaveGameInterface>(GetOwner()))
	{
		if (SaveStruct.SaveValues.Contains(GetOwner()->GetClass()->GetName()))
		{
			SaveGameInterface->RestoreState(SaveStruct.SaveValues[GetOwner()->GetClass()->GetName()]);
		}
	}

	TArray<UActorComponent*> Saveables = GetOwner()->GetComponentsByInterface(USaveGameInterface::StaticClass());
	for (UActorComponent* Saveable : Saveables)
	{
		ISaveGameInterface* SaveGameInterface = Cast<ISaveGameInterface>(Saveable);

		if (SaveStruct.SaveValues.Contains(Saveable->GetClass()->GetName()))
		{
			SaveGameInterface->RestoreState(SaveStruct.SaveValues[Saveable->GetClass()->GetName()]);
		}
	}
}

