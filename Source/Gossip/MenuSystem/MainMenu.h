// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuBase.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UMainMenu : public UMenuBase
{
	GENERATED_BODY()

	UMainMenu(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_Host;
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_Join;
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_Quit;
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_CancelJoinGame;
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_ConfirmJoinGame;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* Switcher;

	UPROPERTY(meta = (BindWidget))
		class UWidget* MainMenu;
	UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
		class UPanelWidget* ScrollBox_ServerList;

	UPROPERTY()
		TArray<UUserWidget*> ServerResults;

protected:
	virtual bool Initialize();

private:
	uint32 SelectedSession;

	UFUNCTION()
		void NewGame();
	UFUNCTION()
		void LoadGame();
	UFUNCTION()
		void OpenMainMenu();
	UFUNCTION()
	void OpenJoinMenu();
	UFUNCTION()
		void QuitGame();

};
