// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerResult.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UServerResult::SetUp(class UMainMenu* Parent, uint32 Index, FString Title)
{
	MainMenu = Parent;
	ButtonIndex = Index;
	Text_ServerName->SetText(FText::FromString(Title));
	if (!BTN) return;
	BTN->OnClicked.AddDynamic(this, &UServerResult::Clicked);
}

void UServerResult::Clicked()
{
	if (!MainMenu) return;
	Text_ServerName->SetColorAndOpacity(FLinearColor::Green);
}
