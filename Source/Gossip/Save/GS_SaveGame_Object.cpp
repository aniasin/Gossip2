// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_SaveGame_Object.h"

UGS_SaveGame_Object::UGS_SaveGame_Object()
{
	SaveGameName = "SaveGame";
	CreationTime = FDateTime::Now();
	SaveData.Empty();
}
