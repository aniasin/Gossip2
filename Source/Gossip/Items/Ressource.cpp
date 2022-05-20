// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/BoxComponent.h"
#include "gossip/NonPlayerCharacter.h"
#include "Gossip/Items/InventoryComponent.h"


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

void ARessource::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Ressource!"))
	ANonPlayerCharacter* NPC = Cast<ANonPlayerCharacter>(OtherActor);
	if (NPC)
	{
		NPC->FoundRessource(this);
		
	}
}

