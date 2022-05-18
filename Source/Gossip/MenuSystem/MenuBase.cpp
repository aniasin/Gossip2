// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuBase.h"

void UMenuBase::SetMenuInterface(IMenuInterface* MenuInterfaceRef)
{
	MenuInterface = MenuInterfaceRef;
}

void UMenuBase::SetUp()
{
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	FInputModeUIOnly InputMode;
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
}

void UMenuBase::TearDown()
{
	this->RemoveFromViewport();

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	FInputModeGameAndUI InputMode;
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
}

void UMenuBase::FoundSessions(TArray<FString> Sessions)
{

}
