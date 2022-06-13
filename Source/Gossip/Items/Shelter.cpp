// Fill out your copyright notice in the Description page of Project Settings.


#include "Shelter.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AShelter::AShelter()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetBoxExtent(FVector(10, 10, 10));

	WallA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall A"));
	WallA->SetupAttachment(RootComponent);
	WallA->SetVisibility(false);

	WallB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall B"));
	WallB->SetupAttachment(RootComponent);
	WallB->SetVisibility(false);

	WallC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall C"));
	WallC->SetupAttachment(RootComponent);
	WallC->SetVisibility(false);

	WallD = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall D"));
	WallD->SetupAttachment(RootComponent);
	WallD->SetVisibility(false);

}
#if WITH_EDITOR
void AShelter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

}
#endif WITH_EDITOR

void AShelter::BeginPlay()
{
	Super::BeginPlay();
	
	WallA->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y, 0));
	WallA->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 60, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallB->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y, 0));
	WallB->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallC->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X * -1, CollisionBox->GetScaledBoxExtent().Y * -1, 0));
	WallC->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().Y / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallD->SetRelativeLocation(FVector(CollisionBox->GetScaledBoxExtent().X, CollisionBox->GetScaledBoxExtent().Y * -1, 0));
	WallD->SetRelativeScale3D(FVector(CollisionBox->GetScaledBoxExtent().X / 50, 0.2, CollisionBox->GetScaledBoxExtent().Z / 100));

	WallA->SetVisibility(true);
	WallB->SetVisibility(true);
	WallC->SetVisibility(true);
	WallD->SetVisibility(true);
	
}
