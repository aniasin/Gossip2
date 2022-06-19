// Fill out your copyright notice in the Description page of Project Settings.


#include "CityHall.h"

#include "Gossip/AI/FamilyComponent.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Core/GossipGameMode.h"
#include "Gossip/Save/SaveableEntity.h"
#include "Gossip/AI/SocialComponent.h"

ACityHall::ACityHall()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SaveComponent = CreateDefaultSubobject<USaveableEntity>(TEXT("SaveComponent"));
}

void ACityHall::BeginPlay()
{
	Super::BeginPlay();
	SaveComponent->Id = Id;
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
		FGuid PretenderGuid = Event.GuestsGuid[0];
		FString Message = FString::Printf(TEXT("%s's Wedding : Time remaining : %s"), *PretenderGuid.ToString(), *TimeRemaining);
		DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
		PosZ += 30;
	}
	FString Message = FString::Printf(TEXT("==== EVENTS ===="));
	DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
}

void ACityHall::NewCityEvent(ECityHallEvents Event, TArray<FGuid>GuestsGuid)
{
	FCityHallEvent CityEvent;
	CityEvent.CityEvent = Event;
	CityEvent.GuestsGuid = GuestsGuid;
	EventsQueued.Add(CityEvent);	
	BeginCityHallEvent(-1);
}

void ACityHall::BeginCityHallEvent(float OverrideTime)
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	float TimeToWait = 0.1;
	if (OverrideTime < 0)
	{
		TimeToWait = GameHoursWaitForEvent * GM->GameHourDurationSeconds;
	}
	if (OverrideTime > 0.2)
	{
		TimeToWait = OverrideTime;
	}

	if (GetWorldTimerManager().IsTimerActive(CityEventTimerHandle)) return;
	for (FCityHallEvent Event : EventsQueued)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ConvokeCityHallEvent", Event);
		GetWorldTimerManager().SetTimer(CityEventTimerHandle, Delegate, TimeToWait, false);
		break;
	}	
}

void ACityHall::ConvokeCityHallEvent(FCityHallEvent Event)
{
	UE_LOG(LogTemp, Warning, TEXT("City Event!"))
	GetWorldTimerManager().ClearTimer(CityEventTimerHandle);
	EventsQueued.RemoveAt(0);
	BeginCityHallEvent(-1);
}

void ACityHall::AddInhabitants(ANonPlayerCharacter* NPC)
{
	UFamilyComponent* FamilyComponent = Cast<UFamilyComponent>(NPC->GetComponentByClass(UFamilyComponent::StaticClass()));
	if (!FamilyComponent) return;
	FamilyComponent->OnNewCityHallEvent.AddDynamic(this, &ACityHall::NewCityEvent);
	Inhabitants.AddUnique(NPC);
}

FSaveValues ACityHall::CaptureState()
{
	FSaveValues SaveValues;

	TMap<float, FCityHallEvent>CurrentEvents;
	int32 Index = 0;
	for (FCityHallEvent Event : EventsQueued)
	{
		if (Index == 0)
		{
			CurrentEvents.Add(GetWorldTimerManager().GetTimerRemaining(CityEventTimerHandle), Event);
		}
		CurrentEvents.Add(GameHoursWaitForEvent, Event);
	}

	SaveValues.CityHallEvents = CurrentEvents;
	return SaveValues;
}

void ACityHall::RestoreState(FSaveValues SaveValues)
{
	for (auto& Event : SaveValues.CityHallEvents)
	{
		EventsQueued.Add(Event.Value);
	}
	TArray<float>TimeRemaining;
	SaveValues.CityHallEvents.GenerateKeyArray(TimeRemaining);
	if (TimeRemaining.Num() > 0)
	{
		BeginCityHallEvent(TimeRemaining[0]);
	}	
}

