// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"


// Sets default values
ARessource::ARessource()
{
	PrimaryActorTick.bCanEverTick = false;


}

void ARessource::BeginPlay()
{
	Super::BeginPlay();

}

void ARessource::CollectRessource(UInventoryComponent* InventoryComp)
{
	// Override in SubClasses
}

