// Fill out your copyright notice in the Description page of Project Settings.


#include "CityHall.h"

#include "Gossip/AI/FamilyComponent.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Core/GossipGameMode.h"

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

void ACityHall::NewCityEvent(ECityHallEvents Event, TArray<AActor*>Guests)
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	FTimerHandle TimerHandle;
	float WaitTime = 1;
	switch (Event)
	{
	case ECityHallEvents::None:
		break;
	case ECityHallEvents::Alarm:
		WaitTime = 0.2;
		break;
	case ECityHallEvents::Wedding:
		WaitTime = GameHoursWaitForWeddings;
		break;
	case ECityHallEvents::Banquet:
		WaitTime = GameHoursWaitForWeddings;
		break;
	}
	FCityHallEvent CityEvent;
	CityEvent.CityEvent = Event;
	CityEvent.Guests = Guests;
	EventsQueue.Add(TimerHandle, CityEvent);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACityHall::BeginCityHallEvent, WaitTime * GM->GameHourDurationSeconds);
}

void ACityHall::BeginCityHallEvent()
{
	TArray<FTimerHandle>Timers;
	EventsQueue.GenerateKeyArray(Timers);

	for (FTimerHandle Timer : Timers)
	{
		if(GetWorldTimerManager().IsTimerActive(Timer)) continue;

		ConvokeCityHallEvent(EventsQueue[Timer].CityEvent, EventsQueue[Timer].Guests);

		GetWorldTimerManager().ClearTimer(Timer);
		EventsQueue.Remove(Timer);
		break;
	}	
}

void ACityHall::ConvokeCityHallEvent(ECityHallEvents Event, TArray<AActor*>Guests)
{
	UE_LOG(LogTemp, Warning, TEXT("City Event!"))
}

void ACityHall::AddInhabitants(ANonPlayerCharacter* NPC)
{
	UFamilyComponent* FamilyComponent = Cast<UFamilyComponent>(NPC->GetComponentByClass(UFamilyComponent::StaticClass()));
	if (!FamilyComponent) return;
	FamilyComponent->OnNewCityHallEvent.AddDynamic(this, &ACityHall::NewCityEvent);
	Inhabitants.AddUnique(NPC);
}

