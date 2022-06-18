// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory.h"
#include "Ressource.h"
#include "CityHall.h"

#include "Gossip/Core/GS_Singleton.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
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

}
#endif WITH_EDITOR

void AShelter::BeginPlay()
{
	Super::BeginPlay();

	ShelterData = ShelterDataAsset->ShelterDataMap[ShelterGrade];
	CurrentLevel = ShelterData.ShelterLevel;

	InitializeShelter();
	SpawnNPC();
}

void AShelter::SpawnNPC()
{
	if (IsValid(NpcToSpawnClass))
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


void AShelter::BeginConstruct()
{
	FStreamableManager& Streamable = UGS_Singleton::Get().AssetLoader;

	TArray<FSoftObjectPath> ItemsPath;
	ItemsPath.AddUnique(ShelterData.ConstructionMeshPath.ToSoftObjectPath());
	for (FSoftObjectPath& Item : ItemsPath)
	{
		Streamable.RequestAsyncLoad(Item, FStreamableDelegate::CreateUObject(this, &AShelter::OnAsyncLoadConstructionComplete));
	}
}

void AShelter::ConstructShelter()
{
	CurrentConstructionStep += 1;
	UE_LOG(LogTemp, Log, TEXT("Shelter has been worked on! Step: %s/%s"), *FString::SanitizeFloat(CurrentConstructionStep), *FString::SanitizeFloat(ShelterData.ConstructionTime));
	if (CurrentConstructionStep >= ShelterData.ConstructionTime)
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
}

void AShelter::SetShelterSize()
{
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
	FSaveValues SaveValues;
	SaveValues.ShelterConstructionStep = CurrentConstructionStep;
	SaveValues.ShelterLevel = CurrentLevel;

	return SaveValues;
}

void AShelter::RestoreState(FSaveValues SaveValues)
{
	CurrentConstructionStep = SaveValues.ShelterConstructionStep;
	CurrentLevel = SaveValues.ShelterLevel;

	if (CurrentConstructionStep > 0) BeginConstruct();

	InitializeShelter();
}

