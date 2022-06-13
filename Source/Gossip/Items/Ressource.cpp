// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"

#include "Gossip/Core/GossipGameMode.h"
#include "Gossip/Data/RessourceDataAsset.h"


ARessource::ARessource()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Overlap);
	CollisionBox->SetBoxExtent(FVector(100, 100, 100));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	SaveGameComp = CreateDefaultSubobject<USaveableEntity>(TEXT("SaveGameComp"));

}

#if WITH_EDITOR
void ARessource::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	URessourceDataAsset* RessourceInfos = Cast<URessourceDataAsset>(RessourceData);
	if (!RessourceInfos) return;

	for (FRessourceData Ressource : RessourceInfos->RessourceDataArray)
	{
		if (RessourceType == EAIGoal::None) return;
		if (Ressource.RessourceType == RessourceType)
		{
			UStaticMesh* MeshPtr;
			MeshPtr = LoadObject<UStaticMesh>(nullptr, *Ressource.MeshPath.ToString());
			if (IsValid(MeshPtr)) { Mesh->SetStaticMesh(MeshPtr); } 
			else { Mesh->SetStaticMesh(nullptr); }
			UAnimMontage* MontagePtr;
			MontagePtr = LoadObject<UAnimMontage>(nullptr, *Ressource.MontagePath.ToString());
			if (IsValid(MontagePtr)) { AnimMontage = MontagePtr; }
			else { AnimMontage = nullptr; }
			bRaw = Ressource.bRaw;
			WaitTime = Ressource.WaitTime;
			ContentCount = Ressource.ContentCount;
			LivingColor = Ressource.LivingColor;
			DeadColor = Ressource.DeadColor;
			RespawnTime = Ressource.RespawnTimeInGameHour;
			break;
		}
	}
}
#endif WITH_EDITOR
void ARessource::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Mesh->GetStaticMesh()))
	{
		UMaterialInterface* Material = Mesh->GetMaterial(0);
		MaterialInstance = Mesh->CreateDynamicMaterialInstance(0, Material);
		MaterialInstance->SetVectorParameterValue("Base Color", LivingColor);
	}
}

void ARessource::CollectRessource(UInventoryComponent* InventoryComp)
{
	// Override in SubClasses
}

void ARessource::AddRessourceAsKnown(UInventoryComponent* InventoryComp)
{
	// Override in SubClasses
}

void ARessource::RessourceEmpty()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;

	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	FTimerHandle Timer;
	float TimerTime = RespawnTime * GM->GameHourDurationSeconds;
	GetWorldTimerManager().SetTimer(Timer, this, &ARessource::RessourceRespawn, TimerTime);
	if (MaterialInstance)
	{
		MaterialInstance->SetVectorParameterValue("Base Color", DeadColor);
	}
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
	if (MaterialInstance)
	{
		MaterialInstance->SetVectorParameterValue("Base Color", LivingColor);
	}
}

UAnimMontage* ARessource::GetAnimMontageMontage()
{
	return AnimMontage;
} 

// ISaveGameInterface
FSaveValues ARessource::CaptureState()
{
	FSaveValues SaveValues;
	SaveValues.ContentCount = ContentCount;

	return SaveValues;
}

void ARessource::RestoreState(FSaveValues SaveData)
{
	ContentCount = SaveData.ContentCount;
	if (ContentCount <= 0) RessourceEmpty();
}
