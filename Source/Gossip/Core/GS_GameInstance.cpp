// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

#include "Gossip/Save/GS_SaveGame_Object.h"
#include "Gossip/Save/SaveableEntity.h"
#include "Gossip/MenuSystem/MenuBase.h"

const static FName SESSION_NAME = TEXT("My Session");

UGS_GameInstance::UGS_GameInstance(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/MenuSystem/BP_MainMenu"));
	if (MainMenuBPClass.Class != NULL)
	{
		MenuClass = MainMenuBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget> GameMenuBPClass(TEXT("/Game/MenuSystem/BP_GameMenu"));
	if (GameMenuBPClass.Class != NULL)
	{
		GameMenuClass = GameMenuBPClass.Class;
		if (!GameMenuClass)	UE_LOG(LogTemp, Warning, TEXT("Could not find GameMenuClass in GameInstance!"));
	}
}

void UGS_GameInstance::Init()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	FString LogMessage;
	OnlineSubsystem != nullptr ? LogMessage = OnlineSubsystem->GetSubsystemName().ToString() : "OnlineSubsystem not found!";
	UE_LOG(LogTemp, Warning, TEXT("Loaded subsystem: %s"), *LogMessage)

		SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface)
	{
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UGS_GameInstance::SessionCreated);
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UGS_GameInstance::SessionDestroyed);
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UGS_GameInstance::FoundSession);
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UGS_GameInstance::JoinSessionComplete);
	}
	else { UE_LOG(LogTemp, Warning, TEXT("Session Interface not found!")) }



	GEngine->OnNetworkFailure().AddUObject(this, &UGS_GameInstance::NetworkError);
}

void UGS_GameInstance::LoadMenu()
{
	if (!MenuClass) return;

	Menu = CreateWidget<UMenuBase>(this, MenuClass);
	if (!Menu) return;
	Menu->SetMenuInterface(this);
	Menu->SetUp();
}

void UGS_GameInstance::LoadGameMenu()
{
	if (!GameMenuClass) return;

	UMenuBase* GameMenu = CreateWidget<UMenuBase>(this, GameMenuClass);
	if (!GameMenu) return;
	GameMenu->SetMenuInterface(this);
	GameMenu->SetUp();
}

void UGS_GameInstance::Host()
{
	if (SessionInterface)
	{
		if (SessionInterface->GetNamedSession(SESSION_NAME))
		{
			DestroySession();
		}
		else
		{
			CreateSession();
		}
	}
}

void UGS_GameInstance::Join(const int32& SessionIndex)
{
	if (SessionIndex >= 0)
	{
		if (Menu) Menu->TearDown();
		SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[SessionIndex]);
	}
}

void UGS_GameInstance::SearchSession()
{
	if (SessionInterface)
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch);
		if (SessionSearch)
		{
			SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
			GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Searching Session... "));
		}
	}
}

void UGS_GameInstance::CreateSession()
{
	if (!SessionInterface) return;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bShouldAdvertise = true;
	SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}

void UGS_GameInstance::DestroySession()
{
	SessionInterface->DestroySession(SESSION_NAME);
}

void UGS_GameInstance::SessionCreated(FName SessionName, bool bSuccess)
{
	if (!bSuccess) { UE_LOG(LogTemp, Warning, TEXT("Failed to create Session!"), *SessionName.ToString()) return; }

	if (Menu) Menu->TearDown();
	UWorld* World = GetWorld();
	if (!World) return;
	World->ServerTravel("Map_01?listen");
}

void UGS_GameInstance::SessionDestroyed(FName SessionName, bool bSuccess)
{
	if (bSuccess)
	{
		CreateSession();
	}
}

void UGS_GameInstance::FoundSession(bool bSuccess)
{
	TArray<FString> Results;
	if (bSuccess && SessionSearch)
	{
		for (const FOnlineSessionSearchResult& SessionSearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Sessions names: %s!"), *SessionSearchResult.GetSessionIdStr())
				Results.Add(SessionSearchResult.Session.OwningUserName);

		}
	}
	else
	{
		Results.Add("No Session Found!");
	}
	Menu->FoundSessions(Results);
}

void UGS_GameInstance::JoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FString Address;
	if (!SessionInterface) return;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect String!"))
	}
	APlayerController* PC = GetFirstLocalPlayerController();
	if (!PC) return;

	PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UGS_GameInstance::NetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	LoadMainMenu();
}

void UGS_GameInstance::LoadMainMenu()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Main Menu... "));
	GetFirstLocalPlayerController()->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UGS_GameInstance::QuitGame()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Quiting... "));
	GetFirstLocalPlayerController()->ConsoleCommand("quit");
}

void UGS_GameInstance::SaveGame()
{
	TMap<FGuid, FSaveStruct>SaveData = LoadGameData();
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		UActorComponent* ActorSaveable = Actor->GetComponentByClass(USaveableEntity::StaticClass());
		if (!ActorSaveable) continue;
		USaveableEntity* SaveableEntity = Cast<USaveableEntity>(ActorSaveable);

		if (!SaveData.Contains(SaveableEntity->Id))
		{
			FSaveStruct NewValues;
			SaveData.Add(SaveableEntity->Id, NewValues);
		}
		FSaveStruct NewValues = SaveableEntity->CaptureState(SaveData[SaveableEntity->Id]);
		SaveData.Add(SaveableEntity->Id, NewValues);
	}
	CreateSaveGame(SaveData);
}

TMap<FGuid, FSaveStruct> UGS_GameInstance::LoadGameData()
{
	TMap<FGuid, FSaveStruct>SaveData;
	USaveGame* SaveGameLoaded = LoadSaveGame("SaveGame");
	if (SaveGameLoaded)
	{
		UGS_SaveGame_Object* SaveGameObject = Cast<UGS_SaveGame_Object>(SaveGameLoaded);
		if (SaveGameObject)
		{
			SaveData = SaveGameObject->SaveData;
		}
	}
	return SaveData;
}

bool UGS_GameInstance::CreateSaveGame(TMap<FGuid, FSaveStruct>SaveData)
{
	if (!CurrentSaveGame)
	{
		USaveGame* NewSaveGame = UGameplayStatics::CreateSaveGameObject(UGS_SaveGame_Object::StaticClass());
		if (NewSaveGame)
		{
			CurrentSaveGame = Cast<UGS_SaveGame_Object>(NewSaveGame);
		}
	}
	else
	{
		CurrentSaveGame->CreateSaveGame("SaveGame");
	}
	CurrentSaveGame->SaveData = SaveData;
	return UGameplayStatics::SaveGameToSlot(CurrentSaveGame, "SaveGame", 0);
}

class USaveGame* UGS_GameInstance::LoadSaveGame(FString Name)
{
	if (!UGameplayStatics::DoesSaveGameExist(Name, 0)) return nullptr;

	return UGameplayStatics::LoadGameFromSlot(Name, 0);
}

void UGS_GameInstance::RestoreGameState()
{
	UE_LOG(LogTemp, Warning, TEXT("Try Restore GameState"))
	TMap<FGuid, FSaveStruct>SaveData = LoadGameData();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		UActorComponent* ActorSaveable = Actor->GetComponentByClass(USaveableEntity::StaticClass());
		if (!ActorSaveable) continue;
		USaveableEntity* SaveableEntity = Cast<USaveableEntity>(ActorSaveable);

		for (auto& Save : SaveData)
		{
			if (Save.Key == SaveableEntity->Id) SaveableEntity->RestoreState(Save.Value);
		}		
	}
}

