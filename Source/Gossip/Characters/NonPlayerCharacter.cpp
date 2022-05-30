// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"

#include "Gossip/AI/GS_AIController.h"
#include "Gossip/AI/SocialComponent.h"
#include "Gossip/AI/InstinctsComponent.h"

#include "Gossip/Items/InventoryComponent.h"

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

	SocialComp = CreateDefaultSubobject<USocialComponent>(TEXT("SocialComp"));
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	InstinctsComp = CreateDefaultSubobject<UInstinctsComponent>(TEXT("InstinctsComp"));
}

// Called when the game starts or when spawned
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AGS_AIController>(Controller);
	AIController->OnAIGoalChanged.AddDynamic(this, &ANonPlayerCharacter::OnAiGoalChanded);
}

// Called every frame
void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString Message; //TODO make UI
	if (bSelected)
	{
		DrawDebugString(GetWorld(), FVector(0, 0, 100), "Selected", this, FColor::Green, DeltaTime);
	}	
}

void ANonPlayerCharacter::OrderMove(FVector Location)
{
	AIController->AIMoveToLocation(Location);
}

void ANonPlayerCharacter::SetMoveSpeed(bool bRunning)
{
	float Speed;
	bRunning ? Speed = RunSpeed : Speed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

// Broadcast from AlignmentComp
void ANonPlayerCharacter::OnAiGoalChanded(bool bRun)
{
	SetMoveSpeed(bRun);
}

