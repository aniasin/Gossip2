// Fill out your copyright notice in the Description page of Project Settings.


#include "Ressource.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"

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
		Mesh->SetMaterial(0, MaterialInstance);
	}
}

void ARessource::CollectRessource(UInventoryComponent* InventoryComp, AActor* Actor)
{
	// Override in SubClasses
	ActorsWorkingOn.Remove(Actor);
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

float ARessource::StartWorking(AActor* Actor)
{
	AGS_AIController* AIController = Cast<AGS_AIController>(Actor);
	float UpdatedWaitTime = WaitTime;

	ActorsWorkingOn.AddUnique(Actor);
	
	if (!RestoredWorkers.IsEmpty() && RestoredWorkers.Contains(AIController))
	{
		UpdatedWaitTime = WaitTime - RestoredWorkers[AIController];
		RestoredWorkers.Remove(AIController);
	}

	float Timer = GetWorld()->GetTimeSeconds();
	Timers.AddUnique(Timer);

	return UpdatedWaitTime;
}

void ARessource::StopWorking(AActor* Actor)
{
	int32 Index = ActorsWorkingOn.Find(Actor);
	ActorsWorkingOn.RemoveAt(Index);
	Timers.RemoveAt(Index);
}

int32 ARessource::GetRessourceDisponibility()
{
	return ContentCount - ActorsWorkingOn.Num();
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
	
	TArray<float>GameHoursTimesWorkedOn;
	TMap<FGuid, float>CurrentTimers;
	if (Timers.Num() > 0)
	{
		for (float Timer : Timers)
		{
			float TimerWorked = GetWorld()->GetTimeSeconds() - Timer;
			GameHoursTimesWorkedOn.Add(TimerWorked / GM->GameHourDurationSeconds);
		}
		
		int32 Index = 0;
		for (AActor* Actor : ActorsWorkingOn)
		{
			AGS_AIController* AIController = Cast<AGS_AIController>(Actor->GetInstigatorController());
			if (AIController)
			{
				CurrentTimers.Add(AIController->Id, GameHoursTimesWorkedOn[Index]);
			}
			Index++;
		}
	}
	SaveValues.RessourceTimers = CurrentTimers;
	SaveValues.ContentCount = ContentCount;
	SaveValues.OwnersIds = OwnersToSave;
	SaveValues.CoolDown = GetWorldTimerManager().GetTimerRemaining(TimerRespawn);

	return SaveValues;
}

void ARessource::RestoreState(FSaveValues SaveData)
{
	TArray<AActor*> Npcs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANonPlayerCharacter::StaticClass(), Npcs);
	for (FGuid SavedId : SaveData.OwnersIds)
	{
		for (AActor* Npc : Npcs)
		{
			ANonPlayerCharacter* NonPlayerCharacter = Cast<ANonPlayerCharacter>(Npc);
			if (!NonPlayerCharacter) continue;
			if (NonPlayerCharacter->Id != Id) continue;
			Owners.AddUnique(Npc);
		}
	}

	if (!SaveData.RessourceTimers.IsEmpty())
	{
		TArray<FGuid>Guids;
		TArray<AActor*>Actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGS_AIController::StaticClass(), Actors);
		SaveData.RessourceTimers.GenerateKeyArray(Guids);
		for (FGuid Guid : Guids)
		{
			for (AActor* Actor : Actors)
			{
				AGS_AIController* AIController = Cast<AGS_AIController>(Actor->GetInstigatorController());
				if (Guid != AIController->Id) continue;
				RestoredWorkers.Add(AIController, SaveData.RessourceTimers[Guid]);
				break;
			}
		}
	}

	ContentCount = SaveData.ContentCount;
	if (ContentCount <= 0)
	{
		RessourceEmpty();
		float TimerTime = SaveData.CoolDown;
		GetWorldTimerManager().SetTimer(TimerRespawn, this, &ARessource::RessourceRespawn, TimerTime);
	}

}
