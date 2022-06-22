// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuBase.h"
#include "GameMenu.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UGameMenu : public UMenuBase
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_Back;
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_MainMenu;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* Switcher;

	UPROPERTY(meta = (BindWidget))
		class UWidget* Main;
	UPROPERTY(meta = (BindWidget))
		class UWidget* PopUp;

	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_Back_1;
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_MainMenu_1;
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN_Save;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TextBlock_Save;

protected:
	virtual bool Initialize();


private:
	UFUNCTION()
		void ResponseNo();
	UFUNCTION()
		void ResponseYes();
	UFUNCTION()
		void Save();
	UFUNCTION()
		void Popup();
};
