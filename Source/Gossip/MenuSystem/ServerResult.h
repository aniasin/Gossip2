// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerResult.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UServerResult : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
		class UButton* BTN;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Text_ServerName;

	void SetUp(class UMainMenu* Parent, uint32 Index, FString Title);

private:
	class UMainMenu* MainMenu;
	uint32 ButtonIndex = -1;

	UFUNCTION()
		void Clicked();
};
