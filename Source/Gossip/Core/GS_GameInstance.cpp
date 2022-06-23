// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GS_GameViewportClient.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"

#include "Gossip/Save/GS_SaveGame_Object.h"
#include "Gossip/Save/SaveableEntity.h"
#include "Gossip/MenuSystem/MenuBase.h"

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
	static ConstructorHelpers::FClassFinder<UUserWidget> SocialRulesMenuBPClass(TEXT("/Game/MenuSystem/BP_SocialRules"));
	if (SocialRulesMenuBPClass.Class != NULL)
	{
		SocialRulesMenuClass = SocialRulesMenuBPClass.Class;
		if (!SocialRulesMenuClass)	UE_LOG(LogTemp, Warning, TEXT("Could not find SocialRulesMenuClass in GameInstance!"));
	}
}

void UGS_GameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UGS_GameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UGS_GameInstance::EndLoadingScreen);

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	FString LogMessage;
	OnlineSubsystem != nullptr ? LogMessage = OnlineSubsystem->GetSubsystemName().ToString() : "OnlineSubsystem not found!";
	UE_LOG(LogTemp, Warning, TEXT("Loaded subsystem: %s"), *LogMessage)

	GEngine->OnNetworkFailure().AddUObject(this, &UGS_GameInstance::NetworkError);
}

void UGS_GameInstance::BeginLoadingScreen(const FString& MapName)
{
	if (!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.MinimumLoadingScreenDisplayTime = 1.5f;
		LoadingScreen.bMoviesAreSkippable = true;
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
		GetMoviePlayer()->WaitForMovieToFinish(false);
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

		FadeScreen(1, true);
	}
}

void UGS_GameInstance::FadeScreen(float Time, bool bToBlack)
{
	UWorld* World = GetWorld();
	UGS_GameViewportClient* GameViewportClient = Cast<UGS_GameViewportClient>(World->GetGameViewport());
	if (GameViewportClient)
	{
		GameViewportClient->Fade(Time, bToBlack);
	}
}

void UGS_GameInstance::EndLoadingScreen(UWorld* World)
{
	FadeScreen(3, false);
}

void UGS_GameInstance::LoadMenu()
{
	if (!MenuClass) return;

	Menu = CreateWidget<UMenuBase>(this, MenuClass);
	if (!Menu) return;
	Menu->SetMenuInterface(this);
	Menu->SetUp(false);
}

void UGS_GameInstance::LoadGameMenu()
{
	if (!GameMenuClass) return;

	UMenuBase* GameMenu = CreateWidget<UMenuBase>(this, GameMenuClass);
	if (!GameMenu) return;
	GameMenu->SetMenuInterface(this);
	GameMenu->SetUp(true);
}

void UGS_GameInstance::LoadSocialRulesMenu()
{
	if (!SocialRulesMenuClass) return;
	if (SocialRulesMenu && SocialRulesMenu->IsInViewport())return;
	SocialRulesMenu = CreateWidget<UMenuBase>(this, SocialRulesMenuClass);
	if (!SocialRulesMenu) return;
	SocialRulesMenu->SetMenuInterface(this);
	SocialRulesMenu->SetUp(false);
}

void UGS_GameInstance::NewGame()
{
	if (Menu) Menu->TearDown();
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnNewGameLoaded");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;
	UGameplayStatics::LoadStreamLevel(this, FName("Map_01"), true, true, LatentInfo);
}

void UGS_GameInstance::LoadGame()
{
	RestoreGameState();
}

void UGS_GameInstance::Travel(FName MapName)
{
	if (Menu) Menu->TearDown();
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnMapLoaded");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;
	UGameplayStatics::LoadStreamLevel(this, MapName, true, true, LatentInfo);

}

void UGS_GameInstance::NetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	LoadMainMenu();
}

void UGS_GameInstance::LoadMainMenu()
{
	FadeScreen(2, true);
	FTimerHandle QuitTimer;
	GetWorld()->GetTimerManager().SetTimer(QuitTimer, this, &UGS_GameInstance::TravelMainMenu, 3);
}

void UGS_GameInstance::TravelMainMenu()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Main Menu... "));
	GetFirstLocalPlayerController()->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UGS_GameInstance::QuitGame()
{
	FTimerHandle QuitTimer;
	GetWorld()->GetTimerManager().SetTimer(QuitTimer, this, &UGS_GameInstance::Quit, .5);
}

void UGS_GameInstance::Quit()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Quiting... "));
	GetFirstLocalPlayerController()->ConsoleCommand("quit");
}

// SaveGame ****************************************************************
void UGS_GameInstance::SaveGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Saving Game..."))
		TMap<FGuid, FSaveStruct>SaveData = LoadGameDataBinary();
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
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
	CreateSaveGameBinary(SaveData);
}

TMap<FGuid, FSaveStruct> UGS_GameInstance::LoadGameDataBinary()
{
	TMap<FGuid, FSaveStruct>SaveData;
	TArray<uint8>OutSaveData;
	if (UGameplayStatics::LoadDataFromSlot(OutSaveData, "SaveGame", 0))
	{
		if (UGS_SaveGame_Object* CurrentSaveGame = Cast<UGS_SaveGame_Object>(UGameplayStatics::LoadGameFromMemory(OutSaveData)))
		{
			SaveData = CurrentSaveGame->SaveData;
			RealGameTimeSeconds = CurrentSaveGame->GameTimeSeconds;
		}
	}
	return SaveData;
}

bool UGS_GameInstance::CreateSaveGameBinary(TMap<FGuid, FSaveStruct>SaveData)
{
	UGS_SaveGame_Object* CurrentSaveGame = Cast<UGS_SaveGame_Object>(UGameplayStatics::CreateSaveGameObject(UGS_SaveGame_Object::StaticClass()));
	if (!CurrentSaveGame) return false;
	CurrentSaveGame->SaveData = SaveData;
	CurrentSaveGame->GameTimeSeconds = GetWorld()->GetTimeSeconds() + RealGameTimeSeconds;

	TArray<uint8>OutSaveData;
	if (!UGameplayStatics::SaveGameToMemory(CurrentSaveGame, OutSaveData)) return false;
	
	return UGameplayStatics::SaveDataToSlot(OutSaveData, "SaveGame", 0);
}

void UGS_GameInstance::RestoreGameState()
{
	Travel("Map_01");
}

void UGS_GameInstance::OnMapLoaded()
{
	FadeScreen(3, false);
	TMap<FGuid, FSaveStruct>SaveData = LoadGameDataBinary();
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		UActorComponent* ActorSaveable = Actor->GetComponentByClass(USaveableEntity::StaticClass());
		if (!ActorSaveable) continue;
		USaveableEntity* SaveableEntity = Cast<USaveableEntity>(ActorSaveable);

		if (SaveData.Contains(SaveableEntity->Id))
		{
			SaveableEntity->RestoreState(SaveData[SaveableEntity->Id]);
		}
	}
}

void UGS_GameInstance::OnNewGameLoaded()
{
	FadeScreen(3, false);
}
