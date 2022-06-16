// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOrdersComponent.h"
#include "NonPlayerCharacter.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Gossip/AI/GS_AIController.h"

UPlayerOrdersComponent::UPlayerOrdersComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerOrdersComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UPlayerOrdersComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPlayerOrdersComponent::StartBoxSelection()
{
	for (ANonPlayerCharacter* Selected : CurrentSelections)
	{
		Selected->SetSelected(false);
	}
	MousePositionAtStart = GetMousePosition();
	bSelectionActive = true;
}

void UPlayerOrdersComponent::EndBoxSelection()
{
	bSelectionActive = false;
	for (ANonPlayerCharacter* Selected : CurrentSelections)
	{
		Selected->SetSelected(true);
	}
}

void UPlayerOrdersComponent::OrderMoveUnderCursor()
{
	FHitResult Hit;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	FVector Location = Hit.Location;

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!NavigationSystem) return;

	for (ANonPlayerCharacter* Selected : CurrentSelections)
	{
		uint8 AIStatus = Selected->GetAIController()->BlackboardComponent->GetValueAsEnum("AIStatus");
		if (AIStatus == (uint8)EAIStatus::Follow || AIStatus == (uint8)EAIStatus::LeadHome) continue;
		UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(GetWorld(), Selected->GetActorLocation(), Location);
		if (!Path->IsValid()) continue;
		Selected->OrderMove(Location);
	}
}

FVector2D UPlayerOrdersComponent::GetMousePosition()
{
	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	return MousePosition;
}

// ISaveGameInterface
FSaveValues UPlayerOrdersComponent::CaptureState()
{
	FSaveValues SaveValues;

	return SaveValues;
}

void UPlayerOrdersComponent::RestoreState(FSaveValues SaveData)
{

}