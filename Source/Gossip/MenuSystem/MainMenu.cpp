// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Blueprint/UserWidget.h"
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
	BTN_Host->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!BTN_Join) return false;
	BTN_Join->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!BTN_Quit) return false;
	BTN_Quit->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	if (!BTN_ConfirmJoinGame) return false;
	BTN_ConfirmJoinGame->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

	if (!BTN_CancelJoinGame) return false;
	BTN_CancelJoinGame->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	return true;
}

void UMainMenu::OpenMainMenu()
{
	Switcher->SetActiveWidget(MainMenu);
}

void UMainMenu::HostServer()
{
	if (!MenuInterface) return;
	MenuInterface->Host();
}

void UMainMenu::OpenJoinMenu()
{
	if (!Switcher || !JoinMenu) return;
	Switcher->SetActiveWidget(JoinMenu);
	SelectedSession = -1;
	MenuInterface->SearchSession();
}

void UMainMenu::JoinServer()
{
	if (!MenuInterface) return;
	MenuInterface->Join(SelectedSession);
}

void UMainMenu::QuitGame()
{
	if (!MenuInterface) return;
	MenuInterface->QuitGame();
}
