// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"


// Sets default values
ARessource::ARessource()
{
	PrimaryActorTick.bCanEverTick = false;


}

#if WITH_EDITOR
void ARessource::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);


}
#endif #WITH_EDITOR

void ARessource::BeginPlay()
{
	Super::BeginPlay();

}

void ARessource::CollectRessource(UInventoryComponent* InventoryComp)
{
	// Override in SubClasses
}

void ARessource::AddRessourceAsKnown(UInventoryComponent* InventoryComp)
{

}
