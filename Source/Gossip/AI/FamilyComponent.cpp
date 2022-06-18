// Fill out your copyright notice in the Description page of Project Settings.


#include "FamilyComponent.h"

UFamilyComponent::UFamilyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UFamilyComponent::BeginPlay()
{
	Super::BeginPlay();

}

// ISaveGameInterface
FSaveValues UFamilyComponent::CaptureState()
{
	FSaveValues SaveValues;

	return SaveValues;
}

void UFamilyComponent::RestoreState(FSaveValues SaveData)
{

}

