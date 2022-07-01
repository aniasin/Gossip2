// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPawn.h"
#include "Gossip/AI/SocialComponent.h"
#include "Gossip/Save/SaveableEntity.h"


AAIPawn::AAIPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	SaveComponent = CreateDefaultSubobject<USaveableEntity>(TEXT("SaveComp"));
	SocialRulesComponent = CreateDefaultSubobject<USocialRulesComponent>(TEXT("SocialRulesComp"));

	if (!Id.IsValid()) { Id.NewGuid(); }
	SaveComponent->Id = Id;
}

void AAIPawn::BeginPlay()
{
	Super::BeginPlay();
	
	SocialRulesComponent->WeddingRule = WeddingRules;
}

