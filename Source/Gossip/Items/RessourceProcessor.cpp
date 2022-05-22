// Fill out your copyright notice in the Description page of Project Settings.


#include "RessourceProcessor.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"
#include "Gossip/AI/InstinctsComponent.h" //For testing

// Sets default values
ARessourceProcessor::ARessourceProcessor()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComponent;
}

void ARessourceProcessor::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARessourceProcessor::OnOverlapBegin);
}

void ARessourceProcessor::ProcessRessource(UInventoryComponent* InventoryComp)
{
	if (InventoryComp)
	{
		FInventoryItem Item;
		Item.Ressource = RessourceType;
		Item.bRaw = false;
		Item.Time = 0;
		InventoryComp->AddOwnedItem(Item);
	}
}

void ARessourceProcessor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(OtherActor->FindComponentByClass(UInventoryComponent::StaticClass()));

	ProcessRessource(InventoryComp); // TODO call from Controller

}



