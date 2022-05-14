// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_GameInstance.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MenuBase.h"

UGS_GameInstance::UGS_GameInstance(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/MenuSystem/BP_MainMenu"));
	if (MainMenuBPClass.Class != NULL)
	{
		MenuClass = MainMenuBPClass.Class;
		if (!MenuClass)	UE_LOG(LogTemp, Warning, TEXT("Could not find MenuClass in GameInstance!"));
	}
}

void UGS_GameInstance::Init()
{

}

void UGS_GameInstance::LoadMenu()
{
	if (!MenuClass) return;

	Menu = CreateWidget<UMenuBase>(this, MenuClass);
	if (!Menu) return;
	Menu->SetMenuInterface(this);
	Menu->SetUp();
}

void UGS_GameInstance::Host()
{

}

void UGS_GameInstance::Join(const int32& SessionIndex)
{

}

void UGS_GameInstance::SearchSession()
{

}

void UGS_GameInstance::CancelSearchSession()
{

}

void UGS_GameInstance::LoadMainMenu()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Main Menu... "));
	GetFirstLocalPlayerController()->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UGS_GameInstance::QuitGame()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Quiting... "));
	GetFirstLocalPlayerController()->ConsoleCommand("quit");
}
