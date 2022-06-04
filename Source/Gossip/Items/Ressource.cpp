// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"

#include "Gossip/Data/RessourceDataAsset.h"


// Sets default values
ARessource::ARessource()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Overlap);
	CollisionBox->SetBoxExtent(FVector(100, 100, 100));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

#if WITH_EDITOR
void ARessource::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	URessourceDataAsset* RessourceInfos = Cast<URessourceDataAsset>(RessourceData);
	if (!RessourceInfos) return;

	for (FRessourceData Ressource : RessourceInfos->RessourceDataArray)
	{
		if (Ressource.RessourceType == RessourceType)
		{
			UStaticMesh* MeshPtr;
			MeshPtr = LoadObject<UStaticMesh>(nullptr, *Ressource.MeshSoftPath.ToString());
			Mesh->SetStaticMesh(MeshPtr);
			bRaw = Ressource.bRaw;
			WaitTime = Ressource.WaitTime;
			ContentCount = Ressource.ContentCount;
			LivingColor = Ressource.LivingColor;
			DeadColor = Ressource.DeadColor;
			RespawnTime = Ressource.RespawnTime;
			AnimMontage = Ressource.Montage;
			break;
		}
	}
}
#endif WITH_EDITOR
void ARessource::BeginPlay()
{
	Super::BeginPlay();

}

void ARessource::CollectRessource(UInventoryComponent* InventoryComp)
{
	// Override in SubClasses
}

void ARessource::AddRessourceAsKnown(UInventoryComponent* InventoryComp)
{

}

void ARessource::RessourceEmpty()
{
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	K2_RessourceStateChanged(DeadColor);
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, this, &ARessource::RessourceRespawn, RespawnTime);
}

void ARessource::RessourceRespawn()
{
	URessourceDataAsset* RessourceInfos = Cast<URessourceDataAsset>(RessourceData);
	if (!RessourceInfos) return;

	for (FRessourceData Ressource : RessourceInfos->RessourceDataArray)
	{
		if (Ressource.RessourceType == RessourceType)
		{
			ContentCount = Ressource.ContentCount;
			break;
		}
	}
	CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	K2_RessourceStateChanged(LivingColor);
}
