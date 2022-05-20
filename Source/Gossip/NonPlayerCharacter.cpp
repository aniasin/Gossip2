// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AI/GS_AIController.h"
#include "AI/AlignmentComponent.h"
#include "Items/InventoryComponent.h"
#include "Items/Ressource.h"

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

	AlignmentComp = CreateDefaultSubobject<UAlignmentComponent>(TEXT("AlignmentComp"));
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
}

// Called when the game starts or when spawned
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AGS_AIController>(Controller);
	AlignmentComp->AIController = AIController;	
	AlignmentComp->InventoryComp = InventoryComp;

	AlignmentComp->OnAIGoalChanged.AddDynamic(this, &ANonPlayerCharacter::OnAiGoalChanded);
}

// Called every frame
void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("Search Food Count: %i"), InventoryComp->GetKnownRessourcesCount(EAIGoal::SearchFood))

	FString Message;
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
	bRunning ? Speed = 500 : Speed = 100;
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ANonPlayerCharacter::FoundRessource(ARessource* RessourceActor)
{
	InventoryComp->AddKnownRessource(RessourceActor);
}

// Broadcast from AlignmentComp
void ANonPlayerCharacter::OnAiGoalChanded(bool bRun)
{
	SetMoveSpeed(bRun);
}

