// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MenuBase.h"

const static FName SESSION_NAME = TEXT("My Session");

UGS_GameInstance::UGS_GameInstance(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/MenuSystem/BP_MainMenu"));
	if (MainMenuBPClass.Class != NULL)
	{
		MenuClass = MainMenuBPClass.Class;
		if (!MenuClass)	UE_LOG(LogTemp, Warning, TEXT("Could not find MenuClass in GameInstance!"));
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
	World->ServerTravel("Map01?listen");
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
