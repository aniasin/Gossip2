// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetAIGoalAndAction.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InstinctsComponent.h"
#include "GS_AIController.h"

#include "Gossip/Core/GossipGameMode.h"
#include "Gossip/Items/InventoryComponent.h"
#include "SocialComponent.h"

UBTService_SetAIGoalAndAction::UBTService_SetAIGoalAndAction(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

void UBTService_SetAIGoalAndAction::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	// Needed for Interval between tick to take effect.	
}

void UBTService_SetAIGoalAndAction::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AGossipGameMode* GM = Cast<AGossipGameMode>(OwnerComp.GetAIOwner()->GetWorld()->GetAuthGameMode());
	if (GM)
	{
		Interval = GM->GameHourDurationSeconds;
		
	}
}

//////////////////////////////////////////////////////////////
bool UBTService_SetAIGoalAndAction::InitializeService(UBehaviorTreeComponent& OwnerComp)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller) return false;
	AIController = Cast<AGS_AIController>(Controller);
	if (!AIController) return false;

	UActorComponent* Inventory = AIController->GetPawn()->FindComponentByClass(UInventoryComponent::StaticClass());
	if (!Inventory) return false;
	InventoryComp = Cast<UInventoryComponent>(Inventory);
	if (!InventoryComp) return false;

	UActorComponent* Instincts = AIController->GetPawn()->FindComponentByClass(UInstinctsComponent::StaticClass());
	if (!Instincts) return false;
	InstinctsComp = Cast<UInstinctsComponent>(Instincts);
	if (!InstinctsComp) return false;

	PreviousGoal = AIController->BlackboardComponent->GetValueAsEnum("Goal");
	PreviousAction = AIController->BlackboardComponent->GetValueAsEnum("Action");
	PreviousAIStatus = AIController->BlackboardComponent->GetValueAsEnum("AIStatus");
	NewGoal = (uint8)EAIGoal::None;
	NewAction = (uint8)EAIAction::None;
	NewAIStatus = (uint8)EAIStatus::None;

	return true;
}

void UBTService_SetAIGoalAndAction::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!InitializeService(OwnerComp)) return;

	StopSearching();
	if (PreviousGoal != (uint8)EAIGoal::None) return;
	if (PreviousAIStatus != (uint8)EAIStatus::None) return;
	SetGoalAndAction();	
	CheckStock();
	SetTravelRoute();
	CallHelp();
	
	AIController->OnAIGoalChanged.Broadcast(0); //Reset speed level to walk
	AIController->BlackboardComponent->SetValueAsEnum("Goal", NewGoal);
	AIController->BlackboardComponent->SetValueAsEnum("Action", NewAction);
	AIController->BlackboardComponent->SetValueAsEnum("AIStatus", NewAIStatus);
}


void UBTService_SetAIGoalAndAction::StopSearching()
{
	if (PreviousAction == (uint8)EAIAction::SearchCollector || PreviousAction == (uint8)EAIAction::SearchProcessor
		|| PreviousAction == (uint8)EAIAction::StockRaw || PreviousAIStatus == (uint8)EAIStatus::SearchSocialize)
	{
		if (!AIController->HasTimeSearchingElapsed()) return;
		
		for (FInstinctValues& Instinct : InstinctsComp->InstinctsInfo)
		{
			if (PreviousAction == (uint8)EAIAction::StockRaw) break;
			if (Instinct.Goal == (EAIGoal)PreviousGoal)
			{
				Instinct.ReportedValue += Instinct.CurrentValue;
				Instinct.CurrentValue = 0;
				continue;
			}
			Instinct.CurrentValue += Instinct.ReportedValue;
			Instinct.ReportedValue = 0;
		}
		InstinctsComp->NotFoundRessources.AddUnique((EAIGoal)PreviousGoal);
		AIController->ResetAI();	
		UE_LOG(LogTemp, Warning, TEXT("TimeSearching Elapsed in SetAIGoal&Action!"))
	}	
	AIController->SetTimeSearching();
}

void UBTService_SetAIGoalAndAction::SetGoalAndAction()
{
	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		float InstinctValue = FMath::Abs(Instinct.CurrentValue);
		if (InstinctValue >= 1)
		{
			NewGoal = (uint8)Instinct.Goal;
			NewAction = (uint8)EAIAction::None;
			break;
		}
		NewGoal = (uint8)EAIGoal::None;
		NewAction = (uint8)EAIAction::None;
	}
	if (NewGoal != (uint8)EAIGoal::None)
	{
		NewAIStatus = (uint8)EAIStatus::None;
		SetAction();
	}
}

void UBTService_SetAIGoalAndAction::SetAction()
{
	EAIInstinct NewInstinct = EAIInstinct::None;

	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		if (Instinct.Goal != (EAIGoal)NewGoal) continue;		
		NewInstinct = Instinct.Instinct;
		break;		
	}	

	int32 OwnedRessourceProcessed = 0;
	int32 OwnedRessourceRaw = 0;

	switch (NewInstinct)
	{
	case EAIInstinct::None:
		break;
	case EAIInstinct::Assimilation:
		switch ((EAIGoal)NewGoal)
		{
		case EAIGoal::Food:
			OwnedRessourceProcessed = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, ERessourceSubType::None, false);
			if (OwnedRessourceProcessed > 0) { NewAction = (uint8)EAIAction::Satisfy;  return; }

			OwnedRessourceRaw = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, ERessourceSubType::None, true);
			if (OwnedRessourceRaw > 0) { NewAction = (uint8)EAIAction::TravelProcessor;  return; }

			NewAction = (uint8)EAIAction::TravelCollector;
			return;
		case EAIGoal::Jerk:
			NewAIStatus = (uint8)EAIStatus::SearchSocialize;
			return;
		case EAIGoal::Leadership:
			NewAction = (uint8)EAIAction::Improve;
			NewAIStatus = (uint8)EAIStatus::Altruism;
			AIController->GetBlackboardComponent()->SetValueAsObject("TargetActor", AIController->GetPawn());
			return;
		}

	case EAIInstinct::Conservation:
		switch ((EAIGoal)NewGoal)
		{
		case EAIGoal::Sleep:
			NewAction = (uint8)EAIAction::TravelCollector;
			return;
		case  EAIGoal::Shelter:
			if (!InventoryComp->ShelterActor) return;
			OwnedRessourceRaw = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, InventoryComp->RessourceForShelter, true);
			if (OwnedRessourceRaw > 0) 
			{
				AIController->BlackboardComponent->SetValueAsObject("TargetActor", InventoryComp->ShelterActor);
				NewAction = (uint8)EAIAction::Improve;
				return;
			}
			else
			{
				NewAction = (uint8)EAIAction::TravelCollector;
				return;
			}
		case EAIGoal::Rest:
			NewAction = (uint8)EAIAction::TravelCollector;
			return;
		}

	case EAIInstinct::Reproduction:
		switch ((EAIGoal)NewGoal)
		{
		case EAIGoal::Sex:
			NewAIStatus = (uint8)EAIStatus::SearchSocialize;
			return;
		case EAIGoal::Children:
			NewAIStatus = (uint8)EAIStatus::SearchSocialize;
		case EAIGoal::HandWork:
			if (!InventoryComp->ShelterActor) return;
			OwnedRessourceRaw = InventoryComp->GetOwnedItemsCount((EAIGoal)NewGoal, InventoryComp->RessourceForShelter, true);
			if (OwnedRessourceRaw > 0)
			{
				AIController->BlackboardComponent->SetValueAsObject("TargetActor", InventoryComp->ShelterActor);
				NewAction = (uint8)EAIAction::Improve;
				return;
			}
			else
			{
				NewAction = (uint8)EAIAction::TravelCollector;
				return;
			}
		}
	}
	NewAction = (uint8)EAIInstinct::None;
}

void UBTService_SetAIGoalAndAction::SetTravelRoute()
{
	if (NewAction == (uint8)EAIAction::TravelCollector)
	{
		TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)NewGoal);
		if (!IsValid(TargetActor)) { NewAction = (uint8)EAIAction::SearchCollector; return;	}
		AIController->BlackboardComponent->SetValueAsObject("TargetActor", TargetActor);
		return;
	}
	if (NewAction == (uint8)EAIAction::TravelProcessor)
	{
		TargetActor = InventoryComp->SearchNearestKnownRessourceProcessor((EAIGoal)NewGoal);
		if (!IsValid(TargetActor)) { NewAction = (uint8)EAIAction::SearchProcessor;	return;	}
		AIController->BlackboardComponent->SetValueAsObject("TargetActor", TargetActor);
		return;
	}
}

void UBTService_SetAIGoalAndAction::CheckStock()
{
	if (NewGoal != ((uint8)EAIGoal::None)) return;

	ERessourceSubType RessourceSubType = ERessourceSubType::None;

	for (FInstinctValues Instinct : InstinctsComp->InstinctsInfo)
	{
		if (InstinctsComp->NotFoundRessources.Contains(Instinct.Goal)) continue;
		if (!Instinct.bRawStockable && !Instinct.bProcessedStockable) continue;
		if (Instinct.bRawStockable && InventoryComp->GetOwnedItemsCount(Instinct.Goal, RessourceSubType, true) < InventoryComp->StockingLimit 
			&& PreviousAction != (uint8)EAIAction::StockProcessed
			|| Instinct.bRawStockable && InventoryComp->GetOwnedItemsCount(Instinct.Goal, RessourceSubType, true) < InventoryComp->StockingLimit 
			&& InventoryComp->GetOwnedItemsCount(Instinct.Goal, RessourceSubType, false) >= InventoryComp->StockingLimit)
		{
			NewAction = (uint8)EAIAction::StockRaw;
			TargetActor = InventoryComp->SearchNearestKnownRessourceCollector((EAIGoal)Instinct.Goal);
			if (IsValid(TargetActor))
			{
				AIController->BlackboardComponent->SetValueAsObject("TargetActor", TargetActor);
			}
			NewGoal = (uint8)Instinct.Goal;
			return;
		}
		if (Instinct.bProcessedStockable && InventoryComp->GetOwnedItemsCount(Instinct.Goal, RessourceSubType, true) >= 0 
			&& InventoryComp->GetOwnedItemsCount(Instinct.Goal, RessourceSubType, false) < 3)
		{
			NewAction = (uint8)EAIAction::StockProcessed;
			TargetActor = InventoryComp->SearchNearestKnownRessourceProcessor((EAIGoal)Instinct.Goal);
			if (IsValid(TargetActor))
			{
				AIController->BlackboardComponent->SetValueAsObject("TargetActor", TargetActor);
			}
			NewGoal = (uint8)Instinct.Goal;
			return;
		}
	}
	NewAction = (uint8)EAIAction::None;
	NewGoal = (uint8)EAIGoal::None;	
}

void UBTService_SetAIGoalAndAction::CallHelp()
{
	if (NewAction != (uint8)EAIAction::Improve) return;
	if (NewGoal == (uint8)EAIGoal::Shelter || NewGoal == (uint8)EAIGoal::Leadership)
	{
		USocialComponent* SocialComp = Cast<USocialComponent>(AIController->GetPawn()->GetComponentByClass(USocialComponent::StaticClass()));

		TArray<AActor*>Subsidiaries = SocialComp->GetKnownOthersWithAlignment(EAlignmentState::Cooperative);
		Subsidiaries.Append(SocialComp->GetKnownOthersWithAlignment(EAlignmentState::Submissive));

		UE_LOG(LogTemp, Log, TEXT("%s %s is calling subsidiaries..."), *SocialComp->CharacterName.FirstName, *SocialComp->CharacterName.LastName)
		if (Subsidiaries.IsEmpty()) return;
		for (AActor* Subsidiary : Subsidiaries)
		{
			USocialComponent* OtherSocialComp = Cast<USocialComponent>(Subsidiary->GetComponentByClass(USocialComponent::StaticClass()));
			if (!OtherSocialComp->GetKnownOthersWithAlignment(EAlignmentState::Masterful).Contains(AIController->GetPawn())
				|| !OtherSocialComp->GetKnownOthersWithAlignment(EAlignmentState::Imperious).Contains(AIController->GetPawn())) continue;

			UE_LOG(LogTemp, Log, TEXT("... Found %s %s!"), *SocialComp->CharacterName.FirstName, *SocialComp->CharacterName.LastName,
				*OtherSocialComp->CharacterName.FirstName, *OtherSocialComp->CharacterName.LastName)
				AGS_AIController* OtherController = Cast<AGS_AIController>(Subsidiary->GetInstigatorController());
			OtherController->ResetAI();

			OtherController->BlackboardComponent->SetValueAsEnum("AIStatus", (uint8)EAIStatus::Altruism);
			OtherController->BlackboardComponent->SetValueAsEnum("Goal", NewGoal);
			OtherController->BlackboardComponent->SetValueAsEnum("Action", (uint8)EAIAction::Improve);
			OtherController->BlackboardComponent->SetValueAsObject("TargetActor", AIController->BlackboardComponent->GetValueAsObject("TargetActor"));
		}
	}
}

