// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
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
	UFUNCTION(BlueprintCallable)
		void LoadGameMenu();

	UFUNCTION()
		void Host() override;
	UFUNCTION()
		void Join(const int32& SessionIndex) override;
	UFUNCTION()
		void SearchSession() override;
	UFUNCTION()
		void LoadMainMenu() override;
	UFUNCTION()
		void QuitGame() override;

	UFUNCTION()
	void SaveGame();
	UFUNCTION()
	void RestoreGameState();


private:
	class UMenuBase* Menu;
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> GameMenuClass;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void CreateSession();
	void DestroySession();

	void SessionCreated(FName SessionName, bool bSuccess);
	void SessionDestroyed(FName SessionName, bool bSuccess);
	void FoundSession(bool bSuccess);
	void JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void NetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	bool CreateSaveGameBinary(TMap<FGuid, FSaveStruct>SaveData);
	TMap<FGuid, FSaveStruct> LoadGameDataBinary();
};
