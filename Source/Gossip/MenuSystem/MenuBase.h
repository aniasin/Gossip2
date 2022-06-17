// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MenuBase.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UMenuBase : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMenuInterface(IMenuInterface* MenuInterfaceRef);

	void SetUp(bool bStartPaused);
	void TearDown();
	virtual void OpenMenu();

	virtual void FoundSessions(TArray<FString> Sessions);

protected:
	IMenuInterface* MenuInterface;
};
