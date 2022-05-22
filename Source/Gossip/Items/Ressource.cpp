// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h" // TODO remove


// Sets default values
ARessource::ARessource()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComponent;
}

void ARessource::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARessource::OnOverlapBegin);
}

void ARessource::CollectRessource(class UInventoryComponent* InventoryComp)
{
	if (InventoryComp)
	{		
		FInventoryItem Item;
		Item.Ressource = RessourceType;
		Item.bRaw = true;
		Item.Time = 0;
		InventoryComp->AddOwnedItem(Item);
	}
}

void ARessource::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(OtherActor->FindComponentByClass(UInventoryComponent::StaticClass()));

	InventoryComp->AddKnownRessource(this);
	CollectRessource(InventoryComp); // TODO call from Controller
}

