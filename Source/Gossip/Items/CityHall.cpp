// Fill out your copyright notice in the Description page of Project Settings.


#include "CityHall.h"
#include "Kismet/GameplayStatics.h"

#include "Gossip/AI/FamilyComponent.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Core/GossipGameMode.h"
#include "Gossip/Save/SaveableEntity.h"
#include "Gossip/AI/SocialComponent.h"
#include "Gossip/AI/GS_AIController.h"

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
	BeginCityHallEvent(0);
}

void ACityHall::BeginCityHallEvent(float OverrideTime)
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	float TimeToWait = CityEventGameHourDuration * GM->GameHourDurationSeconds;
	if (OverrideTime > 0) {	TimeToWait = OverrideTime;	}

	if (EventsQueued.IsEmpty() || GetWorldTimerManager().GetTimerRemaining(CityEventTimerHandle) > 0) return;	
	
	FTimerDelegate ConvokeDelegate;
	ConvokeDelegate.BindUFunction(this, "ConvokeCityHallEvent", EventsQueued[0], 0);
	GetWorldTimerManager().SetTimer(CityEventTimerHandle, ConvokeDelegate, TimeToWait, false);

}

void ACityHall::ConvokeCityHallEvent(FCityHallEvent Event, float OverrideTime)
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	if (!GM) return;

	float TimeRemaining = CityEventGameHourDuration * GM->GameHourDurationSeconds;
	if (OverrideTime > 0) {	TimeRemaining = OverrideTime; }

	for (AActor* Actor : Event.Guests)
	{
		UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Actor->GetComponentByClass(UFamilyComponent::StaticClass()));
		AGS_AIController* AiController = Cast<AGS_AIController>(Actor->GetInstigatorController());
		AiController->ResetAI();
		FamilyComp->CityHallCalling(GetActorLocation());		
	}

	GetWorldTimerManager().ClearTimer(CityEventTimerHandle);
	FTimerDelegate EndDelegate;
	EndDelegate.BindUFunction(this, "EndCityEvent", EventsQueued[0].CityEvent);
	GetWorldTimerManager().SetTimer(OngoingEventTimerHandle, EndDelegate, TimeRemaining, false);
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

	for (AActor* Guest : EventsQueued[0].Guests)
	{
		AGS_AIController* AIController = Cast<AGS_AIController>(Guest->GetInstigatorController());
		AIController->ResetAI();
	}

	EventsQueued.RemoveAt(0);
	// Next Event
	BeginCityHallEvent(0);
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

	TMap<FGuid,FSavedCityHallEvent> OngoingCityEvent;
	bool bOngoingEvent = GetWorldTimerManager().IsTimerActive(OngoingEventTimerHandle);
	if (bOngoingEvent)
	{
		FSavedCityHallEvent OngoingEvent;
		OngoingEvent.CityEvent = EventsQueued[0].CityEvent;
		OngoingEvent.TimeRemaining = GetWorldTimerManager().GetTimerRemaining(OngoingEventTimerHandle);
		UFamilyComponent* PretenderFamilyComp = Cast<UFamilyComponent>(EventsQueued[0].Guests[0]->GetComponentByClass(UFamilyComponent::StaticClass()));
		FGuid Guid = PretenderFamilyComp->Id;
		OngoingCityEvent.Add(Guid, OngoingEvent);
	}

	TMap<FGuid, FSavedCityHallEvent>CityHallEvents;
	for (FCityHallEvent Event : EventsQueued)
	{
		UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Event.Guests[0]->GetComponentByClass(UFamilyComponent::StaticClass()));
		if (!FamilyComp) continue;
		FSavedCityHallEvent CityEvent;
		CityEvent.CityEvent = Event.CityEvent;
		CityEvent.TimeRemaining = GetWorldTimerManager().GetTimerRemaining(CityEventTimerHandle);
		CityHallEvents.Add(FamilyComp->Id, CityEvent);
	}

	SaveValues.OngoingCityEvent = OngoingCityEvent;
	SaveValues.CityHallEvents = CityHallEvents;
	return SaveValues;
}

void ACityHall::RestoreState(FSaveValues SaveValues)
{
	TArray<AActor*>AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), AllActors);

	FCityHallEvent OngoingEvent;
	float OngoingEventTimeRemaining = 0;
	bool bOngoingEvent = !SaveValues.OngoingCityEvent.IsEmpty();
	if (bOngoingEvent)
	{

		for (AActor* Actor : AllActors)
		{
			UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Actor->GetComponentByClass(UFamilyComponent::StaticClass()));
			if (!SaveValues.OngoingCityEvent.Contains(FamilyComp->Id)) continue;
			FGuid PretenderGuid = FamilyComp->Id;
			OngoingEvent.CityEvent = SaveValues.OngoingCityEvent[PretenderGuid].CityEvent;
			OngoingEvent.Guests.Add(Actor);
			OngoingEvent.Guests.Add(FamilyComp->GetCurrentFiancee());

			TArray<AActor*>Friends;
			USocialComponent* SocialComp = Cast<USocialComponent>(Actor->GetComponentByClass(USocialComponent::StaticClass()));
			Friends.Append(SocialComp->GetFriends());
			USocialComponent* FianceeSocialComp = Cast<USocialComponent>(FamilyComp->GetCurrentFiancee()->GetComponentByClass(USocialComponent::StaticClass()));
			Friends.Append(FianceeSocialComp->GetFriends());
			for (AActor* Friend : Friends)
			{
				OngoingEvent.Guests.AddUnique(Friend);
			}
			OngoingEventTimeRemaining = SaveValues.OngoingCityEvent[PretenderGuid].TimeRemaining;
			break;
		}
	}

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
			EventsQueued.Add(CityEvent);
			TimeRemaining = SavedCityEvent.Value.TimeRemaining;
			break;
		}
	}

	if (bOngoingEvent)
	{
		ConvokeCityHallEvent(OngoingEvent, OngoingEventTimeRemaining);
	}
	else if (!EventsQueued.IsEmpty())
	{
		BeginCityHallEvent(TimeRemaining);
	}

}

