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

protected:
	virtual bool Initialize();


private:
	UFUNCTION()
		void ResponseNo();
	UFUNCTION()
		void ResponseYes();
};
