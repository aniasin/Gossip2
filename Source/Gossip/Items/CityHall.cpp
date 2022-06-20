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

	int32 Index = 0;
	TMap<float, FSavedCityHallEvent>CurrentEvents;
	for (auto& Event : EventsQueued)
	{
		FSavedCityHallEvent GuidEvent;
		AActor* Pretender = Event.Guests[0];				
		USocialComponent* SocialComp = Cast<USocialComponent>(Pretender->GetComponentByClass(USocialComponent::StaticClass()));
		GuidEvent.PretenderGuid = SocialComp->Id;		
		GuidEvent.CityEvent = Event.CityEvent;
		if (Index == 0)
		{
			CurrentEvents.Add(GetWorldTimerManager().GetTimerRemaining(CityEventTimerHandle), GuidEvent);
			Index++;
			continue;
		}
		CurrentEvents.Add(GameHoursWaitForEvent, GuidEvent);
	}

	SaveValues.CityHallEvents = CurrentEvents;
	return SaveValues;
}

void ACityHall::RestoreState(FSaveValues SaveValues)
{
	TArray<AActor*>AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), AllActors);

	TArray<FCityHallEvent>CurrentCityHallEvents;
	for (auto& Event : SaveValues.CityHallEvents)
	{
		FCityHallEvent CityHallEvent;
		for (AActor* Actor : AllActors)
		{
			UFamilyComponent* FamilyComp = Cast<UFamilyComponent>(Actor->GetComponentByClass(UFamilyComponent::StaticClass()));
			if (!FamilyComp) continue;
			if (Event.Value.PretenderGuid != FamilyComp->Id) continue;
			CityHallEvent.Guests.AddUnique(Actor);
			CityHallEvent.CityEvent = Event.Value.CityEvent;
			USocialComponent* PretenderSocialComp = Cast<USocialComponent>(Actor->GetComponentByClass(USocialComponent::StaticClass()));
			TArray<AActor*>FriendsToAdd;
			FriendsToAdd.Append(PretenderSocialComp->GetFriends());
			AActor* CurrentFiancee = FamilyComp->GetCurrentFiancee();
			USocialComponent* FianceeSocialComp = Cast<USocialComponent>(CurrentFiancee->GetComponentByClass(USocialComponent::StaticClass()));
			FriendsToAdd.Append(FianceeSocialComp->GetFriends());
			for (AActor* Friend : FriendsToAdd)
			{
				CityHallEvent.Guests.AddUnique(Friend);
			}
			break;
		}		
		CurrentCityHallEvents.Add(CityHallEvent);
	}
	for (auto& Event : CurrentCityHallEvents)
	{
		EventsQueued.Add(Event);
	}

	TArray<float>TimeRemaining;
	SaveValues.CityHallEvents.GenerateKeyArray(TimeRemaining);
	if (TimeRemaining.Num() > 0)
	{
		BeginCityHallEvent(TimeRemaining[0]);
	}	
}

