// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuBase.h"
#include "Kismet/GameplayStatics.h"

void UMenuBase::SetMenuInterface(IMenuInterface* MenuInterfaceRef)
{
	MenuInterface = MenuInterfaceRef;
}

void UMenuBase::SetUp(bool bStartPaused)
{
	this->AddToViewport();

	UWorld* World = GetWorld();
	if (!World) return;
	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;
	UGameplayStatics::SetGamePaused(World, bStartPaused);
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
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
	UGameplayStatics::SetGamePaused(World, false);
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
}

void UMenuBase::OpenMenu()
{

}

void UMenuBase::FoundSessions(TArray<FString> Sessions)
{

}
