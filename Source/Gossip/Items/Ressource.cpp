// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"

#include "Gossip/Core/GS_Singleton.h"
#include "Gossip/AI/GS_AIController.h"
#include "Gossip/Characters/NonPlayerCharacter.h"
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

	if (RessourceSubType == ERessourceSubType::None) {
		UE_LOG(LogTemp, Error, TEXT("RessourceSubType is not set in %s"), *GetName()); return;
	}

	URessourceDataAsset* RessourceInfos = Cast<URessourceDataAsset>(RessourceDataAsset);
	if (!RessourceInfos) return;
	RessourceData = RessourceInfos->RessourceDataMap[RessourceSubType];
	RessourceType = RessourceData.RessourceType;
	
	UStaticMesh* MeshPtr;
	if (!RessourceData.MeshesPtr.IsValidIndex(DiversityIndex)) { DiversityIndex = 0; }
	MeshPtr = LoadObject<UStaticMesh>(nullptr, *RessourceData.MeshesPtr[DiversityIndex].ToString());
	if (IsValid(MeshPtr)) { Mesh->SetStaticMesh(MeshPtr); } 
	else { Mesh->SetStaticMesh(nullptr); }

	UAnimMontage* MontagePtr;
	MontagePtr = LoadObject<UAnimMontage>(nullptr, *RessourceData.MontagePath.ToString());
	if (IsValid(MontagePtr)) { AnimMontage = MontagePtr; }
	else { AnimMontage = nullptr; }

	bRaw = RessourceData.bRaw;
	WaitTime = RessourceData.WaitTime;
	MaxContentCount = RessourceData.ContentCount;
	ContentCount = MaxContentCount;
	LivingColor = RessourceData.LivingColor;
	DeadColor = RessourceData.DeadColor;
	RespawnTime = RessourceData.RespawnTimeInGameHour;
	MaxQuality = RessourceData.MeshesPtr.Num();	
}
#endif WITH_EDITOR

void ARessource::BeginPlay()
{
	Super::BeginPlay();

	URessourceDataAsset* RessourceInfos = Cast<URessourceDataAsset>(RessourceDataAsset);
	if (!RessourceInfos) return;
	RessourceData = RessourceInfos->RessourceDataMap[RessourceSubType];
	RessourceType = RessourceData.RessourceType;

	bRaw = RessourceData.bRaw;
	WaitTime = RessourceData.WaitTime;
	MaxContentCount = RessourceData.ContentCount;
	ContentCount = MaxContentCount;
	LivingColor = RessourceData.LivingColor;
	DeadColor = RessourceData.DeadColor;
	RespawnTime = RessourceData.RespawnTimeInGameHour;
	MaxQuality = RessourceData.MeshesPtr.Num();

	if (IsValid(Mesh->GetStaticMesh()))
	{
		UMaterialInterface* Material = Mesh->GetMaterial(0);
		MaterialInstance = Mesh->CreateDynamicMaterialInstance(0, Material);
		MaterialInstance->SetVectorParameterValue("Base Color", LivingColor);
		Mesh->SetMaterial(0, MaterialInstance);
	}
}

void ARessource::InitializeRessource(int32 Index)
{
	URessourceDataAsset* RessourceInfos = Cast<URessourceDataAsset>(RessourceDataAsset);
	if (!RessourceInfos) return;
	RessourceData = RessourceInfos->RessourceDataMap[RessourceSubType];
	RessourceType = RessourceData.RessourceType;

	UStaticMesh* MeshPtr;
	int32 MeshIndex = Index;
	if (!RessourceData.MeshesPtr.IsValidIndex(MeshIndex)) { MeshIndex = 0; }
	MeshPtr = LoadObject<UStaticMesh>(nullptr, *RessourceData.MeshesPtr[MeshIndex].ToString());
	if (IsValid(MeshPtr)) { Mesh->SetStaticMesh(MeshPtr); }
	else { Mesh->SetStaticMesh(nullptr); }
}

void ARessource::CollectRessource(UInventoryComponent* InventoryComp, AActor* Actor)
{
	// Override in SubClasses
	AGS_AIController* AIController = Cast<AGS_AIController>(Actor->GetInstigatorController());
	if (StoredWorkers.Contains(AIController->Id))
	{
		StoredWorkers.Remove(AIController->Id);
		ActorsWorkingOn.Remove(Actor);
	}
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

	float TimerTime = RespawnTime * GM->GameHourDurationSeconds;
	GetWorldTimerManager().SetTimer(TimerRespawn, this, &ARessource::RessourceRespawn, TimerTime);
	if (MaterialInstance)
	{
		MaterialInstance->SetVectorParameterValue("Base Color", DeadColor);
	}
}

void ARessource::RessourceRespawn()
{
	ContentCount = RessourceData.ContentCount;
	CollisionBox->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	if (MaterialInstance)
	{
		MaterialInstance->SetVectorParameterValue("Base Color", LivingColor);
	}
}

float ARessource::StartWorking(AActor* Controller)
{
	if (bHasDifferentQualities)
	{
		WaitTime -= FMath::Clamp((WaitTime / 100) * DiversityIndex, 0.1, WaitTime);
	}

	float TimeToWait = WaitTime;

	AGS_AIController* AIController = Cast<AGS_AIController>(Controller);
	if (StoredWorkers.Contains(AIController->Id))
	{
		TimeToWait = WaitTime - StoredWorkers[AIController->Id];
	}

	ActorsWorkingOn.AddUnique(Controller);
	float Timer = GetWorld()->GetTimeSeconds();
	Timers.Add(Timer);

	return TimeToWait;
}

void ARessource::StopWorking(AActor* Controller)
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	int32 Index = ActorsWorkingOn.Find(Controller);

	float TimeSpent = GetWorld()->GetTimeSeconds() - Timers[Index];
	if (TimeSpent / GM->GameHourDurationSeconds < WaitTime)
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

int32 ARessource::GetRessourceDisponibility()
{
	return ContentCount - ActorsWorkingOn.Num();
}

void ARessource::IncrementQuality()
{
	bHasDifferentQualities = true;
	if (DiversityIndex >= MaxQuality - 1) return;
	int32 NewIndex = FMath::Clamp(DiversityIndex + 1, 0, MaxQuality);
	DiversityIndex = NewIndex;
	InitializeRessource(DiversityIndex);
}

UAnimMontage* ARessource::GetAnimMontageMontage()
{
	return AnimMontage;
} 

// ISaveGameInterface
FSaveValues ARessource::CaptureState()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());

	FSaveValues SaveValues;
	TArray<FGuid> OwnersToSave;

	for (AActor* OwnerNpc : Owners)
	{
		ANonPlayerCharacter* Npc = Cast<ANonPlayerCharacter>(OwnerNpc);
		if (!Npc) continue;
		OwnersToSave.AddUnique(Npc->Id);
	}
	
	if (!ActorsWorkingOn.IsEmpty())
	{
		for (AActor* Controller : ActorsWorkingOn)
		{
			int32 Index = ActorsWorkingOn.Find(Controller);
			float TimeSpent = GetWorld()->GetTimeSeconds() - Timers[Index];
			if (TimeSpent / GM->GameHourDurationSeconds >= WaitTime) continue;			
			AGS_AIController* AIController = Cast<AGS_AIController>(Controller);
			StoredWorkers.Add(AIController->Id, TimeSpent / GM->GameHourDurationSeconds);
		}
	}
	SaveValues.Transform = GetActorTransform();
	SaveValues.StoredWorkers = StoredWorkers;
	SaveValues.ContentCount = ContentCount;
	SaveValues.Guids = OwnersToSave;
	SaveValues.DiversityIndex = DiversityIndex;
	SaveValues.Boolean = bHasDifferentQualities;
	SaveValues.CoolDown = GetWorldTimerManager().GetTimerRemaining(TimerRespawn);

	return SaveValues;
}

void ARessource::RestoreState(FSaveValues SaveData)
{
	TArray<AActor*> Npcs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANonPlayerCharacter::StaticClass(), Npcs);
	for (FGuid SavedId : SaveData.Guids)
	{
		for (AActor* Npc : Npcs)
		{
			ANonPlayerCharacter* NonPlayerCharacter = Cast<ANonPlayerCharacter>(Npc);
			if (!NonPlayerCharacter) continue;
			if (NonPlayerCharacter->Id != Id) continue;
			Owners.AddUnique(Npc);
			break;
		}
	}
	StoredWorkers = SaveData.StoredWorkers;

	ContentCount = SaveData.ContentCount;
	if (ContentCount <= 0)
	{
		RessourceEmpty();
		float TimerTime = SaveData.CoolDown;
		GetWorldTimerManager().SetTimer(TimerRespawn, this, &ARessource::RessourceRespawn, TimerTime);
	}
	SetActorTransform(SaveData.Transform);

	bHasDifferentQualities = SaveData.Boolean;
	DiversityIndex = SaveData.DiversityIndex;
	InitializeRessource(DiversityIndex);
}
