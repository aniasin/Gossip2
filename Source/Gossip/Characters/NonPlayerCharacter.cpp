// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"

#include "Gossip/Core/GS_Singleton.h"

#include "Gossip/AI/GS_AIController.h"
#include "Gossip/AI/SocialComponent.h"
#include "Gossip/AI/InstinctsComponent.h"

#include "Gossip/Items/InventoryComponent.h"

#include "Gossip/Data/CharactersDataAsset.h"

#include "Gossip/Save/SaveableEntity.h"

// Sets default values
ANonPlayerCharacter::ANonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	SocialComp = CreateDefaultSubobject<USocialComponent>(TEXT("SocialComp"));
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	InstinctsComp = CreateDefaultSubobject<UInstinctsComponent>(TEXT("InstinctsComp"));
	SaveComponent = CreateDefaultSubobject<USaveableEntity>(TEXT("SaveComponent"));
}

// Called when the game starts or when spawned
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharactersData = CharactersDataAsset->CharactersData;

	InitializeCharacterProfile();

	AIController = Cast<AGS_AIController>(Controller);
	AIController->OnAIGoalChanged.AddDynamic(this, &ANonPlayerCharacter::OnAiGoalChanded);
	InstinctsComp->OnInstinctsUpdated.AddDynamic(this, &ANonPlayerCharacter::OnInstinctsUpdate);
}

// Called every frame
void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO make UI
	if (bSelected)
	{
		DrawDebugString(GetWorld(), FVector(0, 0, 100), "Selected", this, FColor::Green, DeltaTime);
// 		float PosZ = 200;
// 		for (auto& KnownOther : SocialComp->GetKnownOthers())
// 		{
// 			FString Message = FString::Printf(TEXT("%s : %i"), *KnownOther.Key, KnownOther.Value.Proximity);
// 			DrawDebugString(GetWorld(), FVector(0, 0, PosZ), Message, this, FColor::Cyan, DeltaTime);
// 			PosZ += 30;
// 		}
// 		DrawDebugString(GetWorld(), FVector(0, 0, PosZ), "KNOWN OTHERS", this, FColor::Cyan, DeltaTime);
	}	
}

void ANonPlayerCharacter::InitializeCharacterProfile()
{
	if (SocialComp->CharacterProfile == ECharacterProfile::None) { UE_LOG(LogTemp, Error, TEXT("%s - Character Profile has not been set"), *GetName()) return; }

	FStreamableManager& Streamable = UGS_Singleton::Get().AssetLoader;

	TArray<FSoftObjectPath> ItemsPath;
	ItemsPath.AddUnique(CharactersData[SocialComp->CharacterProfile].MeshPtr.ToSoftObjectPath());

	for (FSoftObjectPath& Item : ItemsPath)
	{
		Streamable.RequestAsyncLoad(Item, FStreamableDelegate::CreateUObject(this, &ANonPlayerCharacter::OnAsyncLoadComplete));
	}
}

void ANonPlayerCharacter::OnAsyncLoadComplete()
{
	USkeletalMesh* SkeletalMesh = CharactersData[SocialComp->CharacterProfile].MeshPtr.Get();
	if (!IsValid(SkeletalMesh)) { UE_LOG(LogTemp, Error, TEXT("CharacterDataAsset has not been filled!")) return; }

	GetMesh()->SetSkeletalMesh(SkeletalMesh);
	GetMesh()->SetAnimInstanceClass(CharactersData[SocialComp->CharacterProfile].AnimBPClass);
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

	return SaveValues;
}

void ANonPlayerCharacter::RestoreState(FSaveValues SaveValues)
{
	SetActorTransform(SaveValues.Transform);
	AIController->ResetAI();
}

