// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory.h"
#include "Ressource.h"
#include "CityHall.h"

#include "Gossip/Core/GS_Singleton.h"
#include "Gossip/Core/GossipGameMode.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/AI/GS_AIController.h"
#include "Gossip/Items/InventoryComponent.h"
#include "Gossip/Data/ShelterDataAsset.h"
#include "Gossip/Save/SaveableEntity.h"

#include "Engine/StaticMesh.h"

AShelter::AShelter()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Overlap);
	CollisionBox->SetBoxExtent(FVector(300, 300, 300));

	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(CollisionBox);

	Ramp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ramp"));
	Ramp->SetupAttachment(CollisionBox);

	WallA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallA"));
	WallA->SetupAttachment(CollisionBox);
	WallB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallB"));
	WallB->SetupAttachment(CollisionBox);
	WallC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallC"));
	WallC->SetupAttachment(CollisionBox);
	WallD = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallD"));
	WallD->SetupAttachment(CollisionBox);

	PillarA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarA"));
	PillarA->SetupAttachment(CollisionBox);
	PillarB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarB"));
	PillarB->SetupAttachment(CollisionBox);
	PillarC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarC"));
	PillarC->SetupAttachment(CollisionBox);
	PillarD = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarD"));
	PillarD->SetupAttachment(CollisionBox);

	ConstructionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Construction"));
	ConstructionMesh->SetupAttachment(CollisionBox);

	SaveComponent = CreateDefaultSubobject<USaveableEntity>(TEXT("SaveComp"));
	Inventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));
}
#if WITH_EDITOR
void AShelter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Floor->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, CollisionBox->GetScaledBoxExtent().Y / 50, 2));
	Floor->SetRelativeLocation(FVector(- CollisionBox->GetScaledBoxExtent().X, - CollisionBox->GetScaledBoxExtent().Y, -500));

	Ramp->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, -500));
	Ramp->SetRelativeRotation(FRotator(0, -90, 0));
	Ramp->SetRelativeScale3D(FVector(2, 4, 2));

}
#endif WITH_EDITOR

void AShelter::BeginPlay()
{
	Super::BeginPlay();

	ShelterData = ShelterDataAsset->ShelterDataMap[ShelterGrade];
	CurrentLevel = ShelterData.ShelterLevel;
	ConstructionTime = ShelterData.ConstructionTime;

	switch (ShelterGrade)
	{
	case ESocialPosition::Noble:
		RessourceForImprovement = ERessourceSubType::Stone;
		break;
	case ESocialPosition::Bourgeois:
		RessourceForImprovement = ERessourceSubType::Stone;
		break;
	case ESocialPosition::Worker:
		RessourceForImprovement = ERessourceSubType::Wood;
		break;
	case ESocialPosition::Tchandala:
		RessourceForImprovement = ERessourceSubType::Wood;
		break;
	}

	InitializeShelter();
	SpawnNPC();
}

void AShelter::SpawnNPC()
{
	if (NpcToSpawnClass)
	{
		ANonPlayerCharacter* NPC = GetWorld()->SpawnActor<ANonPlayerCharacter>(NpcToSpawnClass, GetActorTransform());
		if (!IsValid(NPC)) return;
		NPC->Id = SpawnedNpcId;

		if (!SleepCollector || !FoodProcessor) { UE_LOG(LogTemp, Warning, TEXT("No Ressource has been set in %s"), *GetName()) return; }
		SleepCollector->Owners.AddUnique(NPC);
		FoodProcessor->Owners.AddUnique(NPC);

		UInventoryComponent* NpcInventory = Cast<UInventoryComponent>(NPC->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (NpcInventory)
		{
			NpcInventory->AddKnownRessourceCollector(SleepCollector);
			NpcInventory->AddKnownRessourceProcessor(FoodProcessor);
			NpcInventory->RessourceForShelter = RessourceForImprovement;
		}

		NPC->SetCharacterProfile(this);
		AddToOwners(NPC);

		RegisterNpcToCityHall(NPC);
	}
}

void AShelter::RegisterNpcToCityHall(ANonPlayerCharacter* NPC)
{
	if (CityHall)
	{
		CityHall->AddInhabitants(NPC);
	}
}

void AShelter::InitializeShelter()
{
	FStreamableManager& Streamable = UGS_Singleton::Get().AssetLoader;

	TArray<FSoftObjectPath> ItemsPath;
	switch (CurrentLevel)
	{
		case 0:
			break;
		case 1:
			ItemsPath.AddUnique(ShelterData.WallMeshesPath.ToSoftObjectPath());
			ItemsPath.AddUnique(ShelterData.PillarMeshesPath.ToSoftObjectPath());
			break;
		case 2:
			break;
		case 3:
			break;
	}

	for (FSoftObjectPath& Item : ItemsPath)
	{
		Streamable.RequestAsyncLoad(Item, FStreamableDelegate::CreateUObject(this, &AShelter::OnAsyncLoadComplete));
	}

	Inventory->StockingLimit = ShelterData.InventoryCapability * ShelterData.ShelterLevel;
}


void AShelter::MoveShelter(AShelter* NewShelter)
{
	FTransform NewTransform = FTransform::Identity;
	FVector FreeLocation = FVector::ZeroVector;

	if (FreeLocation.IsZero())
	{
		FreeLocation = TraceForSpaceInDirection(NewShelter, NewShelter->GetActorForwardVector() * -1);
	}

	if (FreeLocation.IsZero())
	{
		FreeLocation = TraceForSpaceInDirection(NewShelter, NewShelter->GetActorRightVector());
	}

	if (FreeLocation.IsZero())
	{
		FreeLocation = TraceForSpaceInDirection(NewShelter, NewShelter->GetActorRightVector() * -1);
	}
	NewTransform = TraceForTerrainHeight(NewShelter, FreeLocation);

	FTransform RelativeBedTransform = SleepCollector->GetActorTransform().GetRelativeTransform(GetActorTransform());
	FTransform RelativeFireTransform = FoodProcessor->GetActorTransform().GetRelativeTransform(GetActorTransform());
	SetActorRotation(NewTransform.GetRotation());
	SetActorLocation(NewTransform.GetLocation());

	SleepCollector->SetActorLocation(RelativeBedTransform.GetLocation() + NewTransform.GetLocation());
	FoodProcessor->SetActorLocation(RelativeFireTransform.GetLocation() + NewTransform.GetLocation());
}

float AShelter::BeginConstruct(AActor* Controller)
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	LoadConstructionMeshes();

	float TimeToWait = ConstructionTime;
	AGS_AIController* AIController = Cast<AGS_AIController>(Controller);
	if (StoredWorkers.Contains(AIController->Id))
	{
		TimeToWait = ConstructionTime - StoredWorkers[AIController->Id];
	}

	ActorsWorkingOn.AddUnique(Controller);
	float Timer = GetWorld()->GetTimeSeconds();
	Timers.Add(Timer);

	return TimeToWait;
}

void AShelter::StopConstruct(AActor* Controller)
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	int32 Index = ActorsWorkingOn.Find(Controller);

	float TimeSpent = GetWorld()->GetTimeSeconds() - Timers[Index];
	if (TimeSpent / GM->GameHourDurationSeconds < ConstructionTime)
	{
		AGS_AIController* AIController = Cast<AGS_AIController>(Controller);
		if (StoredWorkers.Contains(AIController->Id))
		{
			StoredWorkers[AIController->Id] += TimeSpent / GM->GameHourDurationSeconds;
		}
		else
		{
			StoredWorkers.Add(AIController->Id, TimeSpent / GM->GameHourDurationSeconds);
		}
	}

	ActorsWorkingOn.RemoveAt(Index);
	Timers.RemoveAt(Index);
}

void AShelter::LoadConstructionMeshes()
{
	FStreamableManager& Streamable = UGS_Singleton::Get().AssetLoader;

	TArray<FSoftObjectPath> ItemsPath;
	ItemsPath.AddUnique(ShelterData.ConstructionMeshPath.ToSoftObjectPath());
	for (FSoftObjectPath& Item : ItemsPath)
	{
		Streamable.RequestAsyncLoad(Item, FStreamableDelegate::CreateUObject(this, &AShelter::OnAsyncLoadConstructionComplete));
	}
}

void AShelter::ConstructShelter(AActor* Controller)
{
	AGS_AIController* AIController = Cast<AGS_AIController>(Controller);
	if (StoredWorkers.Contains(AIController->Id))
	{
		StoredWorkers.Remove(AIController->Id);
		ActorsWorkingOn.Remove(Controller);
	}

	CurrentConstructionStep += 1;
	UE_LOG(LogTemp, Log, TEXT("Shelter has been worked on! Step: %s/%s"), *FString::SanitizeFloat(CurrentConstructionStep), *FString::SanitizeFloat(ConstructionTime));
	if (CurrentConstructionStep >= ConstructionTime)
	{
		UpgradeShelter();
	}
}

void AShelter::UpgradeShelter()
{
	ConstructionMesh->SetStaticMesh(nullptr);
	CurrentLevel++;
	UE_LOG(LogTemp, Log, TEXT("Shelter has been Upgraded! New level: %i"), CurrentLevel);
	InitializeShelter();
}

FVector AShelter::TraceForSpaceInDirection(AShelter* NewShelter, FVector Direction)
{
	FHitResult Hit;
	UWorld* World = GetWorld();
	FVector BoxExtent = CollisionBox->GetScaledBoxExtent();
	FVector Start = NewShelter->GetActorLocation();
	FQuat Rotation = NewShelter->GetActorTransform().GetRotation();
	FVector Center = Start + ((BoxExtent * 2) * Direction) + (BoxExtent * GetActorUpVector());
	FCollisionShape Box = FCollisionShape::MakeBox(FVector(BoxExtent.X, BoxExtent.Y, BoxExtent.Z / 2));
	//DrawDebugBox(GetWorld(), Center, FVector(BoxExtent.X, BoxExtent.Y, BoxExtent.Z / 2), Rotation, FColor::Purple, true, -1, 0, 10);

	bool bHit = World->SweepSingleByChannel(Hit, Center, Center, Rotation, ECC_Visibility, Box);
	if (bHit) 
	{
		return FVector::ZeroVector;
	}
	FVector FreeLocation = Center + BoxExtent * (NewShelter->GetActorUpVector() * -1);

	return FreeLocation;
}

FTransform AShelter::TraceForTerrainHeight(AShelter* NewShelter, FVector FreeLocation)
{
	FHitResult Hit;
	UWorld* World = GetWorld();
	FVector BoxExtent = CollisionBox->GetScaledBoxExtent();
	FQuat Rotation = NewShelter->GetActorTransform().GetRotation();
	FCollisionShape Box = FCollisionShape::MakeBox(BoxExtent);
	bool bHit = World->SweepSingleByChannel(Hit, FreeLocation, FreeLocation, Rotation, ECC_Visibility, Box);

	FVector NewLocation = FreeLocation + BoxExtent * (NewShelter->GetActorUpVector() * -1);
	//DrawDebugBox(GetWorld(), Center, FVector(BoxExtent), Rotation, FColor::Purple, true, -1, 0, 10);
	if (bHit)
	{
		NewLocation.Z = Hit.Location.Z;
		DrawDebugSphere(World, Hit.Location, 50, 8, FColor::Red, true);
		UE_LOG(LogTemp, Log, TEXT("%s"), *Hit.GetActor()->GetName())
	}
	FTransform NewTransform;
	NewTransform.SetLocation(NewLocation);
	NewTransform.SetRotation(Rotation);

	return NewTransform;
}

void AShelter::AddToOwners(AActor* Actor)
{
	Owners.AddUnique(Actor);
}

void AShelter::OnAsyncLoadComplete()
{
	UStaticMesh* WallMesh = nullptr;
	UStaticMesh* PillarMesh = nullptr;

	switch (CurrentLevel)
	{
	case 0:
		break;
	case 1:
		WallMesh = ShelterData.WallMeshesPath.Get();
		PillarMesh = ShelterData.PillarMeshesPath.Get();
	case 2:
		break;
	case 3:
		break;
	}

	if (WallMesh)
	{
		WallA->SetStaticMesh(WallMesh);
		WallB->SetStaticMesh(WallMesh);
		WallC->SetStaticMesh(WallMesh);
		WallD->SetStaticMesh(WallMesh);
	}

	if (PillarMesh)
	{
		PillarA->SetStaticMesh(PillarMesh);
		PillarB->SetStaticMesh(PillarMesh);
		PillarC->SetStaticMesh(PillarMesh);
		PillarD->SetStaticMesh(PillarMesh);
	}


	SetShelterSize();
}

void AShelter::OnAsyncLoadConstructionComplete()
{
	UStaticMesh* Mesh = nullptr;
	Mesh = ShelterData.ConstructionMeshPath.Get();
	if (Mesh) ConstructionMesh->SetStaticMesh(Mesh);
	ConstructionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); //TODO: can mess with NavMesh if not on ground
}

void AShelter::SetShelterSize()
{
	Floor->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, CollisionBox->GetScaledBoxExtent().Y / 50, 2));
	Floor->SetRelativeLocation(FVector(-CollisionBox->GetScaledBoxExtent().X, -CollisionBox->GetScaledBoxExtent().Y, -500));

	Ramp->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, -500));
	Ramp->SetRelativeRotation(FRotator(0, -90, 0));
	Ramp->SetRelativeScale3D(FVector(2, 4, 2));

	WallA->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, 0));
	WallA->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 65, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));
	WallB->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y * -1, 0));
	WallB->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));
	WallC->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y, 0));
	WallC->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));
	WallD->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y * -1, 0));
	WallD->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	PillarA->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, 0));
	PillarB->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y * -1 , 0));
	PillarC->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y, 0));
	PillarD->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y * -1, 0));
}

// ISaveGameInterface
FSaveValues AShelter::CaptureState()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	FSaveValues SaveValues;

	if (!ActorsWorkingOn.IsEmpty())
	{
		for (AActor* Controller : ActorsWorkingOn)
		{
			int32 Index = ActorsWorkingOn.Find(Controller);
			float TimeSpent = GetWorld()->GetTimeSeconds() - Timers[Index];
			if (TimeSpent / GM->GameHourDurationSeconds >= ConstructionTime) continue;
			AGS_AIController* AIController = Cast<AGS_AIController>(Controller);
			StoredWorkers.Add(AIController->Id, TimeSpent / GM->GameHourDurationSeconds);
		}
	}
	SaveValues.StoredWorkers = StoredWorkers;
	SaveValues.ShelterConstructionStep = CurrentConstructionStep;
	SaveValues.ShelterLevel = CurrentLevel;
	SaveValues.Transform = GetActorTransform();

	return SaveValues;
}

void AShelter::RestoreState(FSaveValues SaveValues)
{
	SetActorTransform(SaveValues.Transform);
	StoredWorkers = SaveValues.StoredWorkers;
	CurrentConstructionStep = SaveValues.ShelterConstructionStep;
	CurrentLevel = SaveValues.ShelterLevel;

	if (CurrentConstructionStep > 0) LoadConstructionMeshes();
	InitializeShelter();
}

