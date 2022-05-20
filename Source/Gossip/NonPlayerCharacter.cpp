// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "AI/GS_AIController.h"
#include "AI/AlignmentComponent.h"

// Sets default values
ANonPlayerCharacter::ANonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AlignmentComp = CreateDefaultSubobject<UAlignmentComponent>(TEXT("AlignmentComp"));

}

// Called when the game starts or when spawned
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AGS_AIController>(Controller);
}

// Called every frame
void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
