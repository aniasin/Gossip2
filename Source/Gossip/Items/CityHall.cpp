// Fill out your copyright notice in the Description page of Project Settings.


#include "CityHall.h"

#include "Gossip/AI/FamilyComponent.h"

ACityHall::ACityHall()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void ACityHall::BeginPlay()
{
	Super::BeginPlay();

}

void ACityHall::NewCityEvent()
{
	UE_LOG(LogTemp, Warning, TEXT("Event Received!"));
}

void ACityHall::AddInhabitants(ANonPlayerCharacter* NPC)
{
	UFamilyComponent* FamilyComponent = Cast<UFamilyComponent>(NPC->GetComponentByClass(UFamilyComponent::StaticClass()));
	if (!FamilyComponent) return;
	FamilyComponent->OnNewCityHallEvent.AddDynamic(this, &ACityHall::NewCityEvent);
	Inhabitants.AddUnique(NPC);
}

