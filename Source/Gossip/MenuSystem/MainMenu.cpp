// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "ServerResult.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{

}

bool UMainMenu::Initialize()
{
	bool bsuccess = Super::Initialize();
	if (!bsuccess) return false;

	if (!BTN_Host) return false;
	BTN_Host->OnClicked.AddDynamic(this, &UMainMenu::NewGame);

	if (!BTN_Join) return false;
	BTN_Join->OnClicked.AddDynamic(this, &UMainMenu::LoadGame);

	if (!BTN_Quit) return false;
	BTN_Quit->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	if (!BTN_ConfirmJoinGame) return false;

	if (!BTN_CancelJoinGame) return false;
	BTN_CancelJoinGame->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	return true;
}

void UMainMenu::NewGame()
{
	if (!MenuInterface) return;
	MenuInterface->NewGame();
}

void UMainMenu::LoadGame()
{
	if (!MenuInterface) return;
	MenuInterface->LoadGame("SaveGame");
}

void UMainMenu::OpenMainMenu()
{
	Switcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenJoinMenu()
{

}

void UMainMenu::QuitGame()
{
	if (!MenuInterface) return;
	MenuInterface->QuitGame();
}
