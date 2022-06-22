// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"

bool UGameMenu::Initialize()
{
	bool bsuccess = Super::Initialize();
	if (!bsuccess) return false;

	if (!BTN_Back) return false;
	BTN_Back->OnClicked.AddDynamic(this, &UGameMenu::ResponseNo);
	if (!BTN_MainMenu) return false;
	BTN_MainMenu->OnClicked.AddDynamic(this, &UGameMenu::Popup);

	if (!BTN_MainMenu_1) return false;
	BTN_MainMenu_1->OnClicked.AddDynamic(this, &UGameMenu::ResponseYes);
	if (!BTN_Back_1) return false;
	BTN_Back_1->OnClicked.AddDynamic(this, &UGameMenu::ResponseNo);
	if (!BTN_Save) return false;
	BTN_Save->OnClicked.AddDynamic(this, &UGameMenu::Save);

	if (!Switcher) return false;

	return true;
}

void UGameMenu::ResponseNo()
{
	TearDown();
}

void UGameMenu::ResponseYes()
{
	if (!MenuInterface) return;
	TearDown();
	MenuInterface->LoadMainMenu();
}

void UGameMenu::Save()
{
	if (!MenuInterface) return;
	MenuInterface->SaveGame();
	TextBlock_Save->SetText(FText::FromString("Game Saved!"));
	BTN_Save->SetVisibility(ESlateVisibility::Hidden);
}

void UGameMenu::Popup()
{
	Switcher->SetActiveWidget(PopUp);
}
