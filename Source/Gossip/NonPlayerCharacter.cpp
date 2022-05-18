// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "AI/AlignmentComponent.h"

// Sets default values
ANonPlayerCharacter::ANonPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	RedCircle = CreateDefaultSubobject<USphereComponent>(TEXT("RedCircle"));
	RedCircle->SetupAttachment(RootComponent);
	RedCircle->SetSphereRadius(200);

	AlignmentComp = CreateDefaultSubobject<UAlignmentComponent>(TEXT("AlignmentComp"));

}

// Called when the game starts or when spawned
void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	RedCircle->OnComponentBeginOverlap.AddDynamic(this, &ANonPlayerCharacter::OnOverlapBegin);
	RedCircle->OnComponentEndOverlap.AddDynamic(this, &ANonPlayerCharacter::OnOverlapEnd);

}

// Called every frame
void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString Message;
	bSelected ? Message = "True" : Message = "False";
	DrawDebugString(GetWorld(), FVector(0, 0, 100), Message, this, FColor::Green, DeltaTime);
}

void ANonPlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorInVincinity) return;

	if (OtherActor->IsA(ACharacter::StaticClass()) && OtherActor != this)
	{
		ActorInVincinity = OtherActor;
		AlignmentComp->NewActorInVincinity(OtherActor);
	}	
}

void ANonPlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ACharacter::StaticClass()) && OtherActor != this)
	{
		if (ActorInVincinity && OtherActor == ActorInVincinity)
		{
			ActorInVincinity = nullptr;
		}
	}
}

bool ANonPlayerCharacter::HandleRaycast(APlayerController* PlayerController)
{
	if (PlayerController->IsInputKeyDown("LeftMouseButton") && !bRaycastHandled)
	{
		bRaycastHandled = true;
		return true;		
	}
	if (!PlayerController->IsInputKeyDown("LeftMouseButton") && bRaycastHandled)
	{
		bRaycastHandled = false;
		return false;
	}
	return false;
}

