// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenu.h"
#include "Components/Button.h"

bool UGameMenu::Initialize()
{
	bool bsuccess = Super::Initialize();
	if (!bsuccess) return false;

	if (!BTN_Back) return false;
	BTN_Back->OnClicked.AddDynamic(this, &UGameMenu::ResponseNo);

	if (!BTN_MainMenu) return false;
	BTN_MainMenu->OnClicked.AddDynamic(this, &UGameMenu::ResponseYes);

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
