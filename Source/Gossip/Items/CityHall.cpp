// Fill out your copyright notice in the Description page of Project Settings.


#include "CityHall.h"
#include "Kismet/GameplayStatics.h"

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
		ANonPlayerCharacter* PretenderCharacter = Cast<ANonPlayerCharacter>(Event.Guests[0]);
		FString Message = FString::Printf(TEXT("%s %s's Wedding : Time remaining : %s"), *PretenderCharacter->CharacterName.FirstName, 
			*PretenderCharacter->CharacterName.LastName, *TimeRemaining);
		for (AActor* Friend : Event.Guests)
		{
			ANonPlayerCharacter* FriendCharacter = Cast<ANonPlayerCharacter>(Friend);
			FString FriendName = FString::Printf(TEXT(" %s %s "), *FriendCharacter->CharacterName.FirstName, *FriendCharacter->CharacterName.LastName);
			DrawDebugString(GetWorld(), FVector(0, 0, PosZ), FriendName, this, FColor::Cyan, DeltaTime);
			PosZ += 30;
		}
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

	if (GetWorldTimerManager().GetTimerRemaining(CityEventTimerHandle) > 0) return;
	for (FCityHallEvent Event : EventsQueued)
	{
		FTimerDelegate ConvokeDelegate;
		ConvokeDelegate.BindUFunction(this, "ConvokeCityHallEvent", Event);
		GetWorldTimerManager().SetTimer(CityEventTimerHandle, ConvokeDelegate, TimeToWait, false);
		break;
	}	
}

void ACityHall::ConvokeCityHallEvent(FCityHallEvent Event)
{
	UE_LOG(LogTemp, Warning, TEXT("City Event!"))
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	for (AActor* Actor : Event.Guests)
	{
		UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Actor->GetComponentByClass(UFamilyComponent::StaticClass()));
		FamilyComp->CityHallCalling(GetActorLocation());		
	}

	GetWorldTimerManager().ClearTimer(CityEventTimerHandle);
	FTimerDelegate EndDelegate;
	EndDelegate.BindUFunction(this, "EndCityEvent", EventsQueued[0].CityEvent);
	GetWorldTimerManager().SetTimer(OngoingEventTimerHandle, EndDelegate, CityEventGameHourDuration * GM->GameHourDurationSeconds, false);
}

void ACityHall::EndCityEvent(ECityHallEvents Event)
{
	switch (Event)
	{
	case ECityHallEvents::Alarm:
		break;
	case ECityHallEvents::Banquet:
		break;
	case ECityHallEvents::Wedding:
		UFamilyComponent* PretenderFamilyComp = Cast<UFamilyComponent>(EventsQueued[0].Guests[0]->GetComponentByClass(UFamilyComponent::StaticClass()));
		UFamilyComponent* FianceeFamilyComp = Cast<UFamilyComponent>(PretenderFamilyComp->GetCurrentFiancee()->GetComponentByClass(UFamilyComponent::StaticClass()));
		PretenderFamilyComp->Marry();
		FianceeFamilyComp->Marry();
		break;
	}

	EventsQueued.RemoveAt(0);
	// Next Event
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

	TMap<FGuid, FSavedCityHallEvent>EventsToSave;
	for (FCityHallEvent Event : EventsQueued)
	{
		UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Event.Guests[0]->GetComponentByClass(UFamilyComponent::StaticClass()));
		if (!FamilyComp) continue;
		FSavedCityHallEvent CityEvent;
		CityEvent.CityEvent = Event.CityEvent;
		CityEvent.TimeRemaining = GetWorldTimerManager().GetTimerRemaining(CityEventTimerHandle);
		EventsToSave.Add(FamilyComp->Id, CityEvent);
	}

	SaveValues.CityHallEvents = EventsToSave;
	return SaveValues;
}

void ACityHall::RestoreState(FSaveValues SaveValues)
{
	TArray<AActor*>AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), AllActors);

	float TimeRemaining = 0;
	TMap<FGuid, FSavedCityHallEvent>SavedCityEvents = SaveValues.CityHallEvents;
	for (auto& SavedCityEvent : SavedCityEvents)
	{
		for (AActor* Actor : AllActors)
		{
			UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Actor->GetComponentByClass(UFamilyComponent::StaticClass()));
			if (!FamilyComp) continue;
			if (FamilyComp->Id != SavedCityEvent.Key) continue;
			FCityHallEvent CityEvent;
			ECityHallEvents Event = SavedCityEvent.Value.CityEvent;
			CityEvent.CityEvent = Event;
			CityEvent.Guests.Add(Actor);
			CityEvent.Guests.Add(FamilyComp->GetCurrentFiancee());

			TArray<AActor*>Friends;
			USocialComponent* SocialComp = Cast<USocialComponent>(Actor->GetComponentByClass(USocialComponent::StaticClass()));
			Friends.Append(SocialComp->GetFriends());
			USocialComponent* FianceeSocialComp = Cast<USocialComponent>(FamilyComp->GetCurrentFiancee()->GetComponentByClass(USocialComponent::StaticClass()));
			Friends.Append(FianceeSocialComp->GetFriends());
			for (AActor* Friend : Friends)
			{
				CityEvent.Guests.AddUnique(Friend);
			}
			FString EventStr = GetEnumValueAsString("ECityHallEvents", Event);
			TimeRemaining = SavedCityEvent.Value.TimeRemaining;
			UE_LOG(LogTemp, Log, TEXT("LOADED: %s %s %s"), *SavedCityEvent.Key.ToString(), *EventStr, *FString::SanitizeFloat(TimeRemaining))
			EventsQueued.Add(CityEvent);
			break;
		}
	}

	if (!EventsQueued.IsEmpty())
	{
		BeginCityHallEvent(TimeRemaining);
	}

}

