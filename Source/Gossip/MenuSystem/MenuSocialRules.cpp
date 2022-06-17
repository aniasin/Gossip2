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
	BTN_Back->OnClicked.AddDynamic(this, &UMenuSocialRules::ResponseNo);

	if (GetOwningPlayer())
	{
		AGossipGameMode* GM = Cast<AGossipGameMode>(GetOwningPlayer()->GetWorld()->GetAuthGameMode());
		if (!GM) return false;
		ESlateVisibility ButtonVisibility;
		GM->GetWeddingSeenOnce() ? ButtonVisibility = ESlateVisibility::Visible : ButtonVisibility = ESlateVisibility::Hidden;
		BTN_Back->SetVisibility(ButtonVisibility);
	}

	if (!BTN_Confirm) return false;
	BTN_Confirm->OnClicked.AddDynamic(this, &UMenuSocialRules::ResponseYes);

	if (!ComboBox_Family) return false;
	ComboBox_Family->AddOption("None");
	ComboBox_Family->SetSelectedOption("None");
	FamilySystem.Add("None", EFamilySystem::None);
	ComboBox_Family->AddOption("Patriarcal");
	FamilySystem.Add("Patriarcal", EFamilySystem::Patriarcal);
	ComboBox_Family->AddOption("Matriarcal");
	FamilySystem.Add("Matriarcal", EFamilySystem::Matriarcal);
	ComboBox_Family->OnSelectionChanged.AddDynamic(this, &UMenuSocialRules::ComboKeyFamilyChanged);

	
	if (!ComboBox_Wedding) return false;
	ComboBox_Wedding->AddOption("None");
	ComboBox_Wedding->SetSelectedOption("None");
	WeddingSystem.Add("None", EWeddingSystem::None);
	ComboBox_Wedding->AddOption("Monogamy");
	WeddingSystem.Add("Monogamy", EWeddingSystem::Monogamy);
	ComboBox_Wedding->AddOption("Polygamy");
	WeddingSystem.Add("Polygamy", EWeddingSystem::Polygamy);
	ComboBox_Wedding->OnSelectionChanged.AddDynamic(this, &UMenuSocialRules::ComboKeyWeddingChanged);

	

	return true;
}

void UMenuSocialRules::ResponseNo()
{
	TearDown();
}

void UMenuSocialRules::ResponseYes()
{
	USocialRulesComponent* SocialRulesComp = Cast<USocialRulesComponent>(GetOwningPlayer()->GetPawn()->GetComponentByClass(USocialRulesComponent::StaticClass()));
	SocialRulesComp->SetNewWeddingRule(WeddingRule);

	TearDown();
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
