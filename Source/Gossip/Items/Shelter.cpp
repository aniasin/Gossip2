// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory.h"
#include "Ressource.h"
#include "RessourceCollector.h"
#include "RessourceProcessor.h"
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

	Roof = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Roof"));
	Roof->SetupAttachment(CollisionBox);

	WallFront = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallA"));
	WallFront->SetupAttachment(CollisionBox);
	NavModifierFront = CreateDefaultSubobject<UBoxComponent>(TEXT("NavModifierFront"));
	NavModifierFront->SetupAttachment(WallFront);
	NavModifierFront->SetBoxExtent(FVector(50, 50, 100));
	NavModifierFront->bDynamicObstacle = true;


	WallBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallB"));
	WallBack->SetupAttachment(CollisionBox);
	NavModifierBack = CreateDefaultSubobject<UBoxComponent>(TEXT("NavModifierBack"));
	NavModifierBack->SetupAttachment(WallBack);
	NavModifierBack->SetBoxExtent(FVector(50, 50, 100));
	NavModifierBack->bDynamicObstacle = true;

	WallRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallC"));
	WallRight->SetupAttachment(CollisionBox);
	NavModifierRight = CreateDefaultSubobject<UBoxComponent>(TEXT("NavModifierRight"));
	NavModifierRight->SetupAttachment(WallRight);
	NavModifierRight->SetBoxExtent(FVector(50, 50, 100));
	NavModifierRight->bDynamicObstacle = true;

	WallLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallD"));
	WallLeft->SetupAttachment(CollisionBox);
	NavModifierLeft = CreateDefaultSubobject<UBoxComponent>(TEXT("NavModifierLeft"));
	NavModifierLeft->SetupAttachment(WallLeft);
	NavModifierLeft->SetBoxExtent(FVector(50, 50, 100));
	NavModifierLeft->bDynamicObstacle = true;

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

	CollisionBox->SetBoxExtent(FVector(ShelterSizeX, ShelterSizeY, 300));
	SetShelterSize();
}
#endif WITH_EDITOR

void AShelter::BeginPlay()
{
	Super::BeginPlay();

	ShelterData = ShelterDataAsset->ShelterDataMap[ShelterGrade];
	CurrentLevel = ShelterData.ShelterLevel;
	ConstructionTime = ShelterData.ConstructionTime;

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

		if (PlayerRuler == nullptr) PlayerRuler = GetWorld()->GetFirstPlayerController()->GetPawn();
		NPC->InventoryComp->PlayerRuler = PlayerRuler;

		UInventoryComponent* NpcInventory = Cast<UInventoryComponent>(NPC->GetComponentByClass(UInventoryComponent::StaticClass()));
		for (ARessource* Furniture : Furnitures)
		{
			if (!IsValid(Furniture)) continue;
			Furniture->Owners.AddUnique(NPC);
			if (Furniture->IsA(ARessourceCollector::StaticClass())) NpcInventory->AddKnownRessourceCollector(Furniture);			
			if (Furniture->IsA(ARessourceProcessor::StaticClass())) NpcInventory->AddKnownRessourceProcessor(Furniture);
		}
		NpcInventory->RessourceForShelter = RessourceForImprovement;

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
	SetRessouceForImprovement();

	FStreamableManager& Streamable = UGS_Singleton::Get().AssetLoader;

	TArray<FSoftObjectPath> ItemsPath;
	switch (CurrentLevel)
	{
		case 0:
			break;
		case 1:
			ItemsPath.AddUnique(ShelterData.WallMeshesPath.ToSoftObjectPath());
			ItemsPath.AddUnique(ShelterData.PillarMeshesPath.ToSoftObjectPath());
			ItemsPath.AddUnique(ShelterData.RoofMeshesPath.ToSoftObjectPath());
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

	if (!Owners.IsEmpty())
	{
		AGS_AIController* AIController = Cast<AGS_AIController>(Owners[0]->GetInstigatorController());
		AIController->SetHomeLocation(GetActorLocation());
	}
}

void AShelter::SetRessouceForImprovement()
{
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

float AShelter::BeginHandwork(AActor* Controller)
{
	int32 Index = FMath::RandRange(0, Furnitures.Num() - 1);

	if (Furnitures[Index]->GetPossibleUpgrade())
	{
		CurrentImprovingFurniture = Furnitures[Index];
		 return 0.1 * Furnitures[Index]->DiversityIndex;
	}
	return 0.1;
}

void AShelter::StopHandwork(AActor* Controller)
{
	if (!CurrentImprovingFurniture) return;
	CurrentImprovingFurniture->IncrementQuality();
	CurrentImprovingFurniture = nullptr;
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
		if (CurrentLevel >= 3) return;
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

void AShelter::MoveShelter(AShelter* NewShelter)
{
	FTransform NewTransform = FTransform::Identity;
	FVector FreeLocation = FVector::ZeroVector;
	int32 NewShelterWall = 0;
	int32 ShelterWall = 0;

	if (FreeLocation.IsZero())
	{
		ShelterWall = 4;
		NewShelterWall = 3;
		FreeLocation = TraceForSpaceInDirection(NewShelter, NewShelter->GetActorForwardVector() * -1);
	}

	if (FreeLocation.IsZero())
	{
		ShelterWall = 1;
		NewShelterWall = 2;
		FreeLocation = TraceForSpaceInDirection(NewShelter, NewShelter->GetActorRightVector());
	}

	if (FreeLocation.IsZero())
	{
		ShelterWall = 2;
		NewShelterWall = 3;
		FreeLocation = TraceForSpaceInDirection(NewShelter, NewShelter->GetActorRightVector() * -1);
	}
	NewTransform = TraceForTerrainHeight(NewShelter, FreeLocation);

	TMap<ARessource*, FTransform>FurnituresTransforms;
	for (ARessource* Furniture : Furnitures)
	{
		if (!IsValid(Furniture)) continue;
		FurnituresTransforms.Add(Furniture, Furniture->GetActorTransform().GetRelativeTransform(GetActorTransform()));
	}

	SetActorRotation(NewTransform.GetRotation());
	SetActorLocation(NewTransform.GetLocation());

	NewShelter->RemoveWall(NewShelterWall);
	RemoveWall(ShelterWall);

	for (ARessource* Furniture : Furnitures)
	{
		if (!IsValid(Furniture)) continue;
		Furniture->SetActorLocation(FurnituresTransforms[Furniture].GetLocation() + NewTransform.GetLocation());
	}
}

FVector AShelter::TraceForSpaceInDirection(AShelter* NewShelter, FVector Direction)
{
	FHitResult Hit;
	UWorld* World = GetWorld();

	FVector BoxExtent = NewShelter->CollisionBox->GetScaledBoxExtent();
	float BoxExtentOffset = 0;
	Direction == NewShelter->GetActorForwardVector() * -1 ? BoxExtentOffset = BoxExtent.X : BoxExtentOffset = BoxExtent.Y;

	FVector Start = NewShelter->GetActorLocation();
	FQuat Rotation = NewShelter->GetActorTransform().GetRotation();
	FVector Center = Start + ((BoxExtentOffset * 2) * Direction) + (BoxExtent * GetActorUpVector());
	FCollisionShape Box = FCollisionShape::MakeBox(FVector(BoxExtent.X - 50, BoxExtent.Y - 50, BoxExtent.Z / 2));
	DrawDebugBox(GetWorld(), Center, FVector(BoxExtent.X - 50, BoxExtent.Y -50, BoxExtent.Z / 2), Rotation, FColor::Purple, true, -1, 0, 10);

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

	FVector NewLocation = FreeLocation;
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

void AShelter::RemoveWall(int32 WallIndex)
{
	TArray<UStaticMeshComponent*> Walls;
	Walls.Add(nullptr);
	Walls.Add(WallFront);
	Walls.Add(WallBack);
	Walls.Add(WallRight);
	Walls.Add(WallLeft);
	Walls[WallIndex]->SetStaticMesh(nullptr);
	ShelterRemovedWalls.AddUnique(WallIndex);
}

void AShelter::AddToOwners(AActor* Actor)
{
	Owners.AddUnique(Actor);
}

void AShelter::OnAsyncLoadComplete()
{
	UStaticMesh* WallMesh = nullptr;
	UStaticMesh* PillarMesh = nullptr;
	UStaticMesh* RoofMesh = nullptr;

	switch (CurrentLevel)
	{
	case 0:
		break;
	case 1:
		WallMesh = ShelterData.WallMeshesPath.Get();
		PillarMesh = ShelterData.PillarMeshesPath.Get();
		RoofMesh = ShelterData.RoofMeshesPath.Get();
	case 2:
		WallMesh = ShelterData.WallMeshesPath.Get();
		PillarMesh = ShelterData.PillarMeshesPath.Get();
		RoofMesh = ShelterData.RoofMeshesPath.Get();
		break;
	case 3:
		WallMesh = ShelterData.WallMeshesPath.Get();
		PillarMesh = ShelterData.PillarMeshesPath.Get();
		RoofMesh = ShelterData.RoofMeshesPath.Get();
		break;
	}

	if (WallMesh)
	{
		if (!ShelterRemovedWalls.Contains(1)) { WallFront->SetStaticMesh(WallMesh); }
		else { NavModifierFront->SetCollisionEnabled(ECollisionEnabled::NoCollision); }

		if (!ShelterRemovedWalls.Contains(2)) { WallBack->SetStaticMesh(WallMesh); } 
		else { NavModifierBack->SetCollisionEnabled(ECollisionEnabled::NoCollision); }

		if (!ShelterRemovedWalls.Contains(3)) { WallRight->SetStaticMesh(WallMesh); } 
		else { NavModifierRight->SetCollisionEnabled(ECollisionEnabled::NoCollision); }

		if (!ShelterRemovedWalls.Contains(4)) { WallLeft->SetStaticMesh(WallMesh); } 
		else { NavModifierLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision); }
	}

	if (PillarMesh)
	{
		PillarA->SetStaticMesh(PillarMesh);
		PillarB->SetStaticMesh(PillarMesh);
		PillarC->SetStaticMesh(PillarMesh);
		PillarD->SetStaticMesh(PillarMesh);
	}
	if (RoofMesh)
	{
		Roof->SetStaticMesh(RoofMesh);
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
	Floor->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y * -1, -500));

	Ramp->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y * -1, -500));
	Ramp->SetRelativeRotation(FRotator(0, -180, 0));
	Ramp->SetRelativeScale3D(FVector(2, 4, 2));

	Roof->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, CollisionBox->GetScaledBoxExtent().Y / 50, 1));
	Roof->SetRelativeLocation(FVector(0, 0, 0));

	WallFront->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y * -1, -300));
	WallFront->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 65, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallBack->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, -300));
	WallBack->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallRight->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y, -300));
	WallRight->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallLeft->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y * -1, -300));
	WallLeft->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	PillarA->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, -300));
	PillarB->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y * -1, -300));
	PillarC->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y, -300));
	PillarD->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y * -1, -300));

	NavModifierFront->SetRelativeLocation(FVector(50, 0, 0));
	NavModifierBack->SetRelativeLocation(FVector(50, 0, 0));
	NavModifierRight->SetRelativeLocation(FVector(50, 0, 0));
	NavModifierLeft->SetRelativeLocation(FVector(50, 0, 0));
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
	SaveValues.ShelterRemovedWalls = ShelterRemovedWalls;
	SaveValues.Transform = GetActorTransform();

	return SaveValues;
}

void AShelter::RestoreState(FSaveValues SaveValues)
{
	SetActorTransform(SaveValues.Transform);
	StoredWorkers = SaveValues.StoredWorkers;
	CurrentConstructionStep = SaveValues.ShelterConstructionStep;
	CurrentLevel = SaveValues.ShelterLevel;
	ShelterRemovedWalls = SaveValues.ShelterRemovedWalls;

	if (CurrentConstructionStep > 0) LoadConstructionMeshes();
	InitializeShelter();
}

