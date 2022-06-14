// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Gossip/Core/GS_Singleton.h"
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
	CollisionBox->SetBoxExtent(FVector(300, 300, 300));

	WallA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall A"));
	WallA->SetupAttachment(RootComponent);

	WallB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall B"));
	WallB->SetupAttachment(RootComponent);

	WallC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall C"));
	WallC->SetupAttachment(RootComponent);

	WallD = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall D"));
	WallD->SetupAttachment(RootComponent);
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
	
	InitializeShelter();
}

void AShelter::InitializeShelter()
{
	FStreamableManager& Streamable = UGS_Singleton::Get().AssetLoader;

	TArray<FSoftObjectPath> ItemsPath;
	ItemsPath.AddUnique(ShelterData->ShelterDataAsset[ShelterGrade].WallMeshesPath.ToSoftObjectPath());
	for (FSoftObjectPath& Item : ItemsPath)
	{
		Streamable.RequestAsyncLoad(Item, FStreamableDelegate::CreateUObject(this, &AShelter::OnAsyncLoadComplete));
	}
}

void AShelter::OnAsyncLoadComplete()
{
	UStaticMesh* StaticMesh = ShelterData->ShelterDataAsset[ShelterGrade].WallMeshesPath.Get();
	if (!IsValid(StaticMesh)) return;

	WallA->SetStaticMesh(StaticMesh);
	WallB->SetStaticMesh(StaticMesh);
	WallC->SetStaticMesh(StaticMesh);
	WallD->SetStaticMesh(StaticMesh);

	SetShelterSize();
}

void AShelter::SetShelterSize()
{
	WallA->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, 0));
	WallA->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 65, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallB->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y, 0));
	WallB->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallC->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y * -1, 0));
	WallC->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallD->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y * -1, 0));
	WallD->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));
}
