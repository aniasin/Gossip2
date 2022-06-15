// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOrdersComponent.h"

UPlayerOrdersComponent::UPlayerOrdersComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerOrdersComponent::BeginPlay()
{
	Super::BeginPlay();


}

void UPlayerOrdersComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FSaveValues UPlayerOrdersComponent::CaptureState()
{
	FSaveValues SaveValues;

	return SaveValues;
}

void UPlayerOrdersComponent::RestoreState(FSaveValues SaveData)
{

}

