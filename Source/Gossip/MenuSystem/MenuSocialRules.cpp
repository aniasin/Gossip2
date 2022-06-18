// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuSocialRules.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"

#include "Gossip/Characters/SocialRulesComponent.h"
#include "Gossip/Core/GossipGameMode.h"

bool UMenuSocialRules::Initialize()
{
	bool bsuccess = Super::Initialize();
	if (!bsuccess) return false;

	if (!BTN_Back) return false;
	BTN_Back->OnClicked.AddDynamic(this, &UMenuSocialRules::ResponseBack);

	if (GetOwningPlayer())
	{
		AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwningPlayer()->GetWorld()->GetAuthGameMode());
		if (!GM) return false;
		ESlateVisibility ButtonVisibility;
		GM->GetWeddingSeenOnce() ? ButtonVisibility = ESlateVisibility::Visible : ButtonVisibility = ESlateVisibility::Hidden;
		BTN_Back->SetVisibility(ButtonVisibility);
		WeddingRule = GM->GetWeddingRule();

		FString FamilySystemStr = "None";
		switch (WeddingRule.FamilySystem)
		{
		case EFamilySystem::None:
			FamilySystemStr = "None";
			break;
		case EFamilySystem::Free:
			FamilySystemStr = "Free";
			break;
		case EFamilySystem::Patriarcal:
			FamilySystemStr = "Patriarcal";
			break;
		case EFamilySystem::Matriarcal:
			FamilySystemStr = "Matriarcal";
			break;
		}
		FString WeddingSystemStr = "None";
		switch (WeddingRule.WeddingSystem)
		{
		case EWeddingSystem::None:
			WeddingSystemStr = "None";
			break;
		case EWeddingSystem::Polygamy:
			WeddingSystemStr = "Polygamy";
			break;
		case EWeddingSystem::Monogamy:
			WeddingSystemStr = "Monogamy";
			break;
		case EWeddingSystem::Free:
			WeddingSystemStr = "Free";
			break;
		}
		FamilySystem.Add(FamilySystemStr, WeddingRule.FamilySystem);
		WeddingSystem.Add(WeddingSystemStr, WeddingRule.WeddingSystem);

		UE_LOG(LogTemp, Warning, TEXT("SocialMenu set Rules: %s %s"), *FamilySystemStr, *WeddingSystemStr)

		if (!BTN_Confirm) return false;
		BTN_Confirm->OnClicked.AddDynamic(this, &UMenuSocialRules::ResponseConfirm);

		TArray<FString>SelectedValueFamily;
		FamilySystem.GetKeys(SelectedValueFamily);
		if (!ComboBox_Family) return false;
		ComboBox_Family->AddOption("None");
		FamilySystem.Add("None", EFamilySystem::None);
		ComboBox_Family->AddOption("Patriarcal");
		FamilySystem.Add("Patriarcal", EFamilySystem::Patriarcal);
		ComboBox_Family->AddOption("Matriarcal");
		FamilySystem.Add("Matriarcal", EFamilySystem::Matriarcal);
		ComboBox_Family->AddOption("Free");
		FamilySystem.Add("Free", EFamilySystem::Free);
		ComboBox_Family->SetSelectedOption(SelectedValueFamily[0]);
		ComboBox_Family->OnSelectionChanged.AddDynamic(this, &UMenuSocialRules::ComboKeyFamilyChanged);

		TArray<FString>SelectedValueWedding;
		WeddingSystem.GetKeys(SelectedValueWedding);
		if (!ComboBox_Wedding) return false;
		ComboBox_Wedding->AddOption("None");
		WeddingSystem.Add("None", EWeddingSystem::None);
		ComboBox_Wedding->AddOption("Monogamy");
		WeddingSystem.Add("Monogamy", EWeddingSystem::Monogamy);
		ComboBox_Wedding->AddOption("Polygamy");
		WeddingSystem.Add("Polygamy", EWeddingSystem::Polygamy);
		ComboBox_Wedding->AddOption("Free");
		WeddingSystem.Add("Free", EWeddingSystem::Free);
		ComboBox_Wedding->SetSelectedOption(SelectedValueWedding[0]);
		ComboBox_Wedding->OnSelectionChanged.AddDynamic(this, &UMenuSocialRules::ComboKeyWeddingChanged);

		USocialRulesComponent* SocialRulesComponent = Cast <USocialRulesComponent>(GetOwningPlayerPawn()->GetComponentByClass(USocialRulesComponent::StaticClass()));
		if (SocialRulesComponent)
		{
			OnPlayerAnswered.AddDynamic(SocialRulesComponent, &USocialRulesComponent::RequestWedding);
		}
			
	}

	return true;
}

void UMenuSocialRules::OpenMenu()
{
	Super::OpenMenu();

	if (!bOpen) ButtonBack_Event();
}

void UMenuSocialRules::ResponseBack()
{
	OnPlayerAnswered.Broadcast();
	ButtonBack_Event();
}

void UMenuSocialRules::ResponseConfirm()
{
	if (WeddingRule.FamilySystem == EFamilySystem::None || WeddingRule.WeddingSystem == EWeddingSystem::None)
	{
		//TODO message player that a value must be set
	}
	else
	{
		USocialRulesComponent* SocialRulesComp = Cast<USocialRulesComponent>(GetOwningPlayer()->GetPawn()->GetComponentByClass(USocialRulesComponent::StaticClass()));
		SocialRulesComp->SetNewWeddingRule(WeddingRule);

		if (!BTN_Back->IsVisible()) BTN_Back->SetVisibility(ESlateVisibility::Visible);
		ResponseBack();
	}

}

void UMenuSocialRules::ComboKeyFamilyChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!FamilySystem.Contains(SelectedItem)) return;
	WeddingRule.FamilySystem = FamilySystem[SelectedItem];
}

void UMenuSocialRules::ComboKeyWeddingChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!WeddingSystem.Contains(SelectedItem)) return;
	WeddingRule.WeddingSystem = WeddingSystem[SelectedItem];

}
