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
	UFUNCTION(BlueprintCallable)
	void LoadSocialRulesMenu();

	// IMenuInterface
	UFUNCTION()
	void NewGame() override;
	UFUNCTION()
	void LoadGame() override;
	UFUNCTION()
	void LoadMainMenu() override;
	UFUNCTION()
	void QuitGame() override;


	UFUNCTION()
	void SaveGame();
	UFUNCTION()
	void RestoreGameState();

	UFUNCTION()
	void OnMapLoaded();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RealGameTimeSeconds;

private:
	class UMenuBase* Menu;
	UMenuBase* SocialRulesMenu;
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> GameMenuClass;
	TSubclassOf<class UUserWidget> SocialRulesMenuClass;

	void Travel(FName MapName);

	void NetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	bool CreateSaveGameBinary(TMap<FGuid, FSaveStruct>SaveData);
	TMap<FGuid, FSaveStruct> LoadGameDataBinary();
};
