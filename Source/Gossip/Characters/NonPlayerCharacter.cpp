// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DataTable.h"

#include "Gossip/Core/GS_Singleton.h"
#include "Gossip/AI/GS_AIController.h"
#include "Gossip/AI/SocialComponent.h"
#include "Gossip/AI/InstinctsComponent.h"
#include "Gossip/AI/FamilyComponent.h"
#include "Gossip/Items/InventoryComponent.h"
#include "Gossip/Data/CharactersDataAsset.h"
#include "Gossip/Save/SaveableEntity.h"


ANonPlayerCharacter::ANonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

//	SocialComp = CreateDefaultSubobject<USocialComponent>(TEXT("SocialComp"));
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	InstinctsComp = CreateDefaultSubobject<UInstinctsComponent>(TEXT("InstinctsComp"));
	FamilyComp = CreateDefaultSubobject<UFamilyComponent>(TEXT("FamilyComp"));
	SaveComponent = CreateDefaultSubobject<USaveableEntity>(TEXT("SaveComponent"));
}

void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float PosZ = 0;
	//TODO make UI
	if (bSelected)
	{
// 		for (auto& KnownOther : SocialComp->GetKnownOthers())
// 		{
// 			FString Message = FString::Printf(TEXT("%s : %i"), *KnownOther.Key.ToString(), KnownOther.Value.Proximity);
// 			DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
// 			PosZ += 30;
// 		}
// 		FString Message = FString::Printf(TEXT("==== %s %s ===="), *CharacterName.FirstName, *CharacterName.LastName);
// 		DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
// 	}
// 
		for (AActor* Friend : SocialComp->GetFriends())
		{
			ANonPlayerCharacter* FriendCharacter = Cast<ANonPlayerCharacter>(Friend);
			FString Message = FString::Printf(TEXT("%s %s"), *FriendCharacter->CharacterName.FirstName, *FriendCharacter->CharacterName.LastName);
			DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
			PosZ += 30;
		}
		FString Message = FString::Printf(TEXT("==== FRIENDS ===="));
		DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
	}	
}

void ANonPlayerCharacter::SetCharacterProfile(AActor* ShelterActor)
{
	SaveComponent->Id = Id;
	SocialComp->Id = Id;
	FamilyComp->Id = Id;

	ECharacterProfile ProfileToSet = ECharacterProfile::None;
	float RandomFloat = FMath::RandRange(0, 1);
	RandomFloat > 0.51 ? ProfileToSet = ECharacterProfile::Female : ProfileToSet = ECharacterProfile::Male;
	SocialComp->CharacterProfile = ProfileToSet;

	InventoryComp->ShelterActor = ShelterActor;

	CharactersData = CharactersDataAsset->CharactersData;


	AIController = Cast<AGS_AIController>(Controller);
	AIController->OnAIGoalChanged.AddDynamic(this, &ANonPlayerCharacter::OnAiGoalChanded);
	InstinctsComp->OnInstinctsUpdated.AddDynamic(this, &ANonPlayerCharacter::OnInstinctsUpdate);

	InitializeCharacterProfile();
}

void ANonPlayerCharacter::InitializeCharacterProfile()
{
	if (SocialComp->CharacterProfile == ECharacterProfile::None) { UE_LOG(LogTemp, Error, TEXT("%s - Character Profile has not been set"), *GetName()) return; }

	AIController->Id = Id;

	FStreamableManager& Streamable = UGS_Singleton::Get().AssetLoader;

	TArray<FSoftObjectPath> ItemsPath;
	ItemsPath.AddUnique(CharactersData[SocialComp->CharacterProfile].MeshPtr.ToSoftObjectPath());

	for (FSoftObjectPath& Item : ItemsPath)
	{
		Streamable.RequestAsyncLoad(Item, FStreamableDelegate::CreateUObject(this, &ANonPlayerCharacter::OnAsyncLoadComplete));
	}

	InitializeCharacterName();

}

void ANonPlayerCharacter::InitializeCharacterName()
{
	if (CharacterName.FirstName != "") return;
	ECharacterProfile CharacterProfile = SocialComp->CharacterProfile;
	if (NamesDatatable)
	{
		int32 Min = 0;
		int32 Max = 0;

		switch (CharacterProfile)
		{
		case ECharacterProfile::None:
			break;
		case ECharacterProfile::Male:
			Min = 0;
			Max = 999;
			break;
		case ECharacterProfile::Female:
			Min = 1000;
			Max = 1999;
			break;
		default:
			break;
		}
		int32 RandomInt = FMath::RandRange(Min, Max);

		const FName RowName = FName(FString::FromInt(RandomInt));
		FCharacterName* Row = NamesDatatable->FindRow<FCharacterName>(RowName, "FirstName");
		if (Row)
		{
			CharacterName.FirstName = *Row->FirstName;
			CharacterName.LastName = *Row->LastName;
		}
	}
}

void ANonPlayerCharacter::OnAsyncLoadComplete()
{
	USkeletalMesh* SkeletalMesh = CharactersData[SocialComp->CharacterProfile].MeshPtr.Get();
	if (!IsValid(SkeletalMesh)) { UE_LOG(LogTemp, Error, TEXT("CharacterDataAsset has not been filled!")) return; }

	GetMesh()->SetSkeletalMesh(SkeletalMesh);
	GetMesh()->SetAnimInstanceClass(CharactersData[SocialComp->CharacterProfile].AnimBPClass);
}

ECharacterProfile ANonPlayerCharacter::GetCharacterGender()
{
	return SocialComp->CharacterProfile;
}

void ANonPlayerCharacter::OrderMove(FVector Location)
{
	AIController->AIMoveToLocation(Location);
}

void ANonPlayerCharacter::SetMoveSpeed(int32 SpeedLevel)
{
	float Speed = 0;
	SpeedLevel == 0 ? Speed = WalkSpeed : Speed = FastWalkSpeed;
	SpeedLevel == 2 ? Speed = RunSpeed : Speed = Speed;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

// Broadcast from AlignmentComp
void ANonPlayerCharacter::OnAiGoalChanded(int32 SpeedLevel)
{
	SetMoveSpeed(SpeedLevel);
}

// Broadcast from Controller
void ANonPlayerCharacter::OnInstinctsUpdate(TArray<EAIGoal> HungryInstincts)
{
	SocialComp->UpdateEmotionalState(HungryInstincts);
}

// ISaveGameInterface
FSaveValues ANonPlayerCharacter::CaptureState()
{
	FSaveValues SaveValues;

	SaveValues.Transform = GetActorTransform();
	SaveValues.CharacterProfile = SocialComp->CharacterProfile;
	SaveValues.CharacterName = CharacterName;
	return SaveValues;
}

void ANonPlayerCharacter::RestoreState(FSaveValues SaveValues)
{
	SetActorTransform(SaveValues.Transform);
	SocialComp->CharacterProfile = SaveValues.CharacterProfile;
	CharacterName = SaveValues.CharacterName;
	InitializeCharacterProfile();
	AIController->ResetAI();
}

