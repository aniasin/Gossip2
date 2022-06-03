// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Singleton.h"

UGS_Singleton::UGS_Singleton()
{

}

UGS_Singleton& UGS_Singleton::Get()
{
	UGS_Singleton* Singleton = Cast<UGS_Singleton>(GEngine->GameSingleton);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		return *NewObject<UGS_Singleton>(UGS_Singleton::StaticClass());
	}
}

bool UGS_Singleton::Tick(float DeltaTime)
{
	return true;
}
