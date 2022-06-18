// Fill out your copyright notice in the Description page of Project Settings.


#include "FamilyComponent.h"
#include "SocialComponent.h"

#include "Gossip/AI/GS_AIController.h" // Don't like this one to sit here
#include "Gossip/Core/GossipGameMode.h"

UFamilyComponent::UFamilyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UFamilyComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Only the Pretender will call that.
void UFamilyComponent::RequestWedding(AActor* Other, FWeddingRule WeddingRule)
{
	UFamilyComponent* OtherFamilyComp = Cast<UFamilyComponent>(Other->GetComponentByClass(UFamilyComponent::StaticClass()));

	CurrentWeddingRules = WeddingRule;
	if (WeddingRule.WeddingSystem == EWeddingSystem::Monogamy && Spouses.Num() > 0 || IsValid(CurrentFiancee)) 
	{ 
	ResetOwnerAI();
	OtherFamilyComp->ResetOwnerAI();
	return; 
	}
	
	ScheduleWedding(Other);
	if (OtherFamilyComp)
	{
		OtherFamilyComp->ScheduleWedding(GetOwner());
	}
	OnNewCityHallEvent.Broadcast();
}

void UFamilyComponent::ScheduleWedding(AActor* Fiancee)
{
	CurrentFiancee = Fiancee;
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwner()->GetWorld()->GetAuthGameMode());
	if (!GM || !CurrentFiancee) { ResetOwnerAI(); return; }

	GetOwner()->GetWorldTimerManager().SetTimer(ScheduledWeddingTimerHandle, this, &UFamilyComponent::StartWedding, GameHoursCoolDownBetweenWeddings * GM->GameHourDurationSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Wedding between %s and %s is now scheduled in %s GameHours!"), 
	*GetOwner()->GetName(), *CurrentFiancee->GetName(), *FString::SanitizeFloat(GameHoursCoolDownBetweenWeddings));
	ResetOwnerAI();
}

void UFamilyComponent::StartWedding()
{
	UE_LOG(LogTemp, Warning, TEXT("It's time for %s and %s to marry!"), *GetOwner()->GetName(), *CurrentFiancee->GetName())
}


void UFamilyComponent::ResetOwnerAI()
{
	AGS_AIController* AIController = Cast<AGS_AIController>(GetOwner()->GetInstigatorController());
	if (!AIController) return;
	AIController->ResetAI();
}

// ISaveGameInterface
FSaveValues UFamilyComponent::CaptureState()
{
	FSaveValues SaveValues;

	return SaveValues;
}

void UFamilyComponent::RestoreState(FSaveValues SaveData)
{

}

