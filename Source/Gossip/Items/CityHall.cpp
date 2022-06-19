// Fill out your copyright notice in the Description page of Project Settings.


#include "CityHall.h"

#include "Gossip/AI/FamilyComponent.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Core/GossipGameMode.h"

ACityHall::ACityHall()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

void ACityHall::BeginPlay()
{
	Super::BeginPlay();

}

void ACityHall::Tick(float DeltaTime)
{
	float PosZ = 0;
	int32 Index = 0;
	for (FCityHallEvent Event : EventsQueued)
	{
		FString TimeRemaining = "Waiting";
		if (Index == 0)
		{
			TimeRemaining = FString::SanitizeFloat(GetWorldTimerManager().GetTimerRemaining(CityEventTimerHandle));
		}
		Index++;

		FString Message = FString::Printf(TEXT(" Wedding : Time remaining : %s"), *TimeRemaining);
		DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
		PosZ += 30;
	}
	FString Message = FString::Printf(TEXT("==== EVENTS ===="));
	DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
}

void ACityHall::NewCityEvent(ECityHallEvents Event, TArray<AActor*>Guests)
{
	FCityHallEvent CityEvent;
	CityEvent.CityEvent = Event;
	CityEvent.Guests = Guests;
	EventsQueued.Add(CityEvent);	
	BeginCityHallEvent();
}

void ACityHall::BeginCityHallEvent()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;
	if (GetWorldTimerManager().IsTimerActive(CityEventTimerHandle)) return;
	for (FCityHallEvent Event : EventsQueued)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ConvokeCityHallEvent", Event);
		GetWorldTimerManager().SetTimer(CityEventTimerHandle, Delegate, GameHoursWaitForWeddings* GM->GameHourDurationSeconds, false);
		break;
	}	
}

void ACityHall::ConvokeCityHallEvent(FCityHallEvent Event)
{
	UE_LOG(LogTemp, Warning, TEXT("City Event!"))
	GetWorldTimerManager().ClearTimer(CityEventTimerHandle);
	EventsQueued.RemoveAt(0);
	BeginCityHallEvent();
}

void ACityHall::AddInhabitants(ANonPlayerCharacter* NPC)
{
	UFamilyComponent* FamilyComponent = Cast<UFamilyComponent>(NPC->GetComponentByClass(UFamilyComponent::StaticClass()));
	if (!FamilyComponent) return;
	FamilyComponent->OnNewCityHallEvent.AddDynamic(this, &ACityHall::NewCityEvent);
	Inhabitants.AddUnique(NPC);
}

