// Fill out your copyright notice in the Description page of Project Settings.


#include "FamilyComponent.h"
#include "SocialComponent.h"

#include "Gossip/Core/GossipGameMode.h"

UFamilyComponent::UFamilyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UFamilyComponent::RequestWedding(AActor* Other, FWeddingRule WeddingRule)
{
	CurrentWeddingRules = WeddingRule;
	if (WeddingRule.FamilySystem == EFamilySystem::None || WeddingRule.WeddingSystem == EWeddingSystem::None) return; //TODO ResetAI()
	if (WeddingRule.WeddingSystem == EWeddingSystem::Monogamy && Spouses.Num() > 0) return; //TODO ResetAI()
	if (IsValid(CurrentFiancee)) return;
	
	ScheduleWedding(Other);
	UFamilyComponent* OtherFamilyComp = Cast<UFamilyComponent>(Other->GetComponentByClass(UFamilyComponent::StaticClass()));
	if (OtherFamilyComp)
	{
		OtherFamilyComp->ScheduleWedding(GetOwner());
	}
}

void UFamilyComponent::ScheduleWedding(AActor* Fiancee)
{
	CurrentFiancee = Fiancee;
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwner()->GetWorld()->GetAuthGameMode());
	if (!GM || !CurrentFiancee) return;	

	GetOwner()->GetWorldTimerManager().SetTimer(ScheduledWeddingTimerHandle, this, &UFamilyComponent::StartWedding, GameHoursCoolDownBetweenWeddings * GM->GameHourDurationSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Wedding between %s and %s is now scheduled in %s GameHours!"), 
	*GetOwner()->GetName(), *CurrentFiancee->GetName(), *FString::SanitizeFloat(GameHoursCoolDownBetweenWeddings));
}

void UFamilyComponent::StartWedding()
{

	UE_LOG(LogTemp, Warning, TEXT("It's time for %s and %s to marry!"), *GetOwner()->GetName(), *CurrentFiancee->GetName())
}

void UFamilyComponent::BeginPlay()
{
	Super::BeginPlay();

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

