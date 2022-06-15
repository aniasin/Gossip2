// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory.h"
#include "Ressource.h"

#include "Gossip/Core/GS_Singleton.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
#include "Gossip/Data/ShelterDataAsset.h"

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
	WallA->SetupAttachment(RootComponent);
	WallB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallB"));
	WallB->SetupAttachment(RootComponent);
	WallC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallC"));
	WallC->SetupAttachment(RootComponent);
	WallD = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallD"));
	WallD->SetupAttachment(RootComponent);

	PillarA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarA"));
	PillarA->SetupAttachment(RootComponent);
	PillarB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarB"));
	PillarB->SetupAttachment(RootComponent);
	PillarC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarC"));
	PillarC->SetupAttachment(RootComponent);
	PillarD = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarD"));
	PillarD->SetupAttachment(RootComponent);

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
		NPC->Id = Id;
		NPC->SetCharacterProfile(this);
		if (!SleepCollector || !FoodProcessor) { UE_LOG(LogTemp, Warning, TEXT("No Ressource has been set in %s"), *GetName()) return; }
		SleepCollector->Owners.AddUnique(NPC);
		FoodProcessor->Owners.AddUnique(NPC);
	}
}

void AShelter::InitializeShelter()
{
	WallA->SetStaticMesh(nullptr);
	WallB->SetStaticMesh(nullptr);
	WallC->SetStaticMesh(nullptr);
	WallD->SetStaticMesh(nullptr);

	PillarA->SetStaticMesh(nullptr);
	PillarB->SetStaticMesh(nullptr);
	PillarC->SetStaticMesh(nullptr);
	PillarD->SetStaticMesh(nullptr);

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
	CurrentLevel++;
	UE_LOG(LogTemp, Log, TEXT("Shelter has been Upgraded! New level: %i"), CurrentLevel);
	InitializeShelter();
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
