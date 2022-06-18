// Fill out your copyright notice in the Description page of Project Settings.


#include "FamilyComponent.h"
#include "SocialComponent.h"

UFamilyComponent::UFamilyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UFamilyComponent::RequestWedding(TArray<AActor*>Couple, FWeddingRule WeddingRule)
{
	if (WeddingRule.WeddingSystem == EWeddingSystem::Monogamy && Spouses.Num() > 0) return;
	
	ConfirmWedding(Couple[1]);
	UFamilyComponent* OtherFamilyComp = Cast<UFamilyComponent>(Couple[1]->GetComponentByClass(UFamilyComponent::StaticClass()));
	if (OtherFamilyComp)
	{
		OtherFamilyComp->ConfirmWedding(Couple[0]);
	}
}

void UFamilyComponent::ConfirmWedding(AActor* Spouse)
{
	UE_LOG(LogTemp, Warning, TEXT(" %s is now married with %s!"), *GetOwner()->GetName(), *Spouse->GetName());
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

