// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Gossip/MenuSystem/MenuInterface.h"
#include "GS_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UGS_GameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
public:

	UGS_GameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init();

	UFUNCTION(BlueprintCallable)
		void LoadMenu();

	UFUNCTION()
		void Host() override;
	UFUNCTION()
		void Join(const int32& SessionIndex) override;
	UFUNCTION()
		void SearchSession() override;
	UFUNCTION()
		void CancelSearchSession() override;
	UFUNCTION()
		void LoadMainMenu() override;
	UFUNCTION()
		void QuitGame() override;

private:
	class UMenuBase* Menu;
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> GameMenuClass;
};
