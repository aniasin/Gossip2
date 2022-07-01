// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GS_GameViewportClient.h"
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"

#include "Gossip/Characters/PlayerPawn.h"
#include "Gossip/Characters/AIPawn.h"
#include "GossipGameMode.h"
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

	MapList.Add("Map_03");
	//MapList.Add("Map_01");
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

		FadeScreen(.1, true);
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
	if (IsValid(SocialRulesMenu) && SocialRulesMenu->IsInViewport()) return;
	SocialRulesMenu = CreateWidget<UMenuBase>(this, SocialRulesMenuClass);
	if (!SocialRulesMenu) return;
	SocialRulesMenu->SetMenuInterface(this);
	SocialRulesMenu->SetUp(false);
}

void UGS_GameInstance::NewGame()
{
	if (Menu) Menu->TearDown();
	FadeScreen(0, true);
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnNewGameLoaded");
	LatentInfo.Linkage = 0;
	LatentInfo.UUID = 0;

	int32 RandomIndexInRange = FMath::RandRange(0, MapList.Num() - 1);
	Map = MapList[RandomIndexInRange];
	UGameplayStatics::LoadStreamLevel(this, FName(Map), true, true, LatentInfo);
}

void UGS_GameInstance::LoadGame(FString SaveName)
{
	CurrentSaveName = SaveName;
	RestoreGameState(CurrentSaveName);
}

void UGS_GameInstance::Travel(FName MapName)
{
	if (Menu) Menu->TearDown();
	FadeScreen(0, true);
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("OnGameLoaded");
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
	FadeScreen(1, true);
	FTimerHandle QuitTimer;
	GetWorld()->GetTimerManager().ClearTimer(AutoSaveTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(QuitTimer, this, &UGS_GameInstance::TravelMainMenu, 3);
}

void UGS_GameInstance::TravelMainMenu()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Main Menu... "));
	GetFirstLocalPlayerController()->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
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
void UGS_GameInstance::SaveGame(FString SaveName)
{
	UE_LOG(LogTemp, Warning, TEXT("Saving Game... %s"), *SaveName)
		TMap<FGuid, FSaveStruct>SaveData;

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
	CreateSaveGameBinary(SaveData, SaveName);
}

TMap<FGuid, FSaveStruct> UGS_GameInstance::LoadGameDataBinary(FString SaveName)
{
	UGS_SaveGame_Object* CurrentSaveGame = nullptr;
	UGS_SaveGame_Object* AutoSaveGame = nullptr;
	FDateTime CurrentSaveDate;
	FDateTime AutoSaveDate;
	TMap<FGuid, FSaveStruct>SaveData;
	TMap<FGuid, FSaveStruct>AutoSaveData;
	TArray<uint8>OutSaveData;

	UGameplayStatics::LoadDataFromSlot(OutSaveData, SaveName, 0);	
	CurrentSaveGame = Cast<UGS_SaveGame_Object>(UGameplayStatics::LoadGameFromMemory(OutSaveData));
	if (CurrentSaveGame)
	{
		CurrentSaveDate = CurrentSaveGame->CreationTime;
	}
	
	UGameplayStatics::LoadDataFromSlot(OutSaveData, "AutoSave", 0);		
	AutoSaveGame = Cast<UGS_SaveGame_Object>(UGameplayStatics::LoadGameFromMemory(OutSaveData));
	if (AutoSaveGame)
	{
		AutoSaveDate = AutoSaveGame->CreationTime;
	}			
	if (CurrentSaveGame && CurrentSaveDate >= AutoSaveDate)
	{
		SaveData = CurrentSaveGame->SaveData;
		RealGameTimeSeconds = CurrentSaveGame->GameTimeSeconds;
		Map = CurrentSaveGame->MapName;
		UE_LOG(LogTemp, Warning, TEXT("Loaded %s"), *CurrentSaveGame->SaveGameName)
		return SaveData;
	}
	else if (AutoSaveGame)
	{
		AutoSaveData = AutoSaveGame->SaveData;
		RealGameTimeSeconds = AutoSaveGame->GameTimeSeconds;
		Map = AutoSaveGame->MapName;
		UE_LOG(LogTemp, Warning, TEXT("Loaded %s"), *AutoSaveGame->SaveGameName)
		return AutoSaveData;
	}
	UE_LOG(LogTemp, Warning, TEXT("No SaveGame, creating new Game!"))
	return SaveData;
}

bool UGS_GameInstance::CreateSaveGameBinary(TMap<FGuid, FSaveStruct>SaveData, FString SaveName)
{
	UGS_SaveGame_Object* CurrentSaveGame = Cast<UGS_SaveGame_Object>(UGameplayStatics::CreateSaveGameObject(UGS_SaveGame_Object::StaticClass()));
	if (!CurrentSaveGame) return false;
	CurrentSaveGame->SaveData = SaveData;
	CurrentSaveGame->SaveGameName = SaveName;
	CurrentSaveGame->CreationTime = FDateTime::Now();
	CurrentSaveGame->GameTimeSeconds = GetWorld()->GetTimeSeconds() + RealGameTimeSeconds;
	CurrentSaveGame->MapName = Map;

	TArray<uint8>OutSaveData;
	if (!UGameplayStatics::SaveGameToMemory(CurrentSaveGame, OutSaveData)) return false;
	
	return UGameplayStatics::SaveDataToSlot(OutSaveData, SaveName, 0);
}

void UGS_GameInstance::RestoreGameState(FString SaveName)
{
	CurrentSaveName = SaveName;
	LoadGameDataBinary(CurrentSaveName); // This set Map
	Travel(FName(Map));
}

void UGS_GameInstance::OnGameLoaded()
{
	TMap<FGuid, FSaveStruct>SaveData = LoadGameDataBinary(CurrentSaveName);

	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	GM->CumulatedRealGameTime = RealGameTimeSeconds;

	// Restore Player First
	TArray<AActor*> PlayerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), PlayerActors);
	for (AActor* Player : PlayerActors)
	{
		APlayerPawn* PlayerPawn = Cast<APlayerPawn>(Player);
		if (PlayerPawn)
		{
			USaveableEntity* PlayerSaveComp = Cast<USaveableEntity>(Player->GetComponentByClass(USaveableEntity::StaticClass()));
			if (SaveData.Contains(PlayerPawn->Id))
			{
				PlayerSaveComp->RestoreState(SaveData[PlayerPawn->Id]);
			}
		}
		AAIPawn* AIPlayer = Cast<AAIPawn>(Player);
		if (AIPlayer)
		{
			USaveableEntity* AISaveComp = Cast<USaveableEntity>(AIPlayer->GetComponentByClass(USaveableEntity::StaticClass()));
			if (SaveData.Contains(AIPlayer->Id))
			{
				AISaveComp->RestoreState(SaveData[AIPlayer->Id]);
			}
		}
	}

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		if (Actor->IsA(APlayerPawn::StaticClass())) continue;
		UActorComponent* ActorSaveable = Actor->GetComponentByClass(USaveableEntity::StaticClass());
		if (!ActorSaveable) continue;
		USaveableEntity* SaveableEntity = Cast<USaveableEntity>(ActorSaveable);

		if (SaveData.Contains(SaveableEntity->Id))
		{
			SaveableEntity->RestoreState(SaveData[SaveableEntity->Id]);
		}
	}
	FadeScreen(3, false);
	FTimerDelegate AutoSaveDelegate;
	FString AutoSaveName = "AutoSave";
	AutoSaveDelegate.BindUFunction(this, "SaveGame", AutoSaveName);
	GetWorld()->GetTimerManager().SetTimer(AutoSaveTimerHandle, AutoSaveDelegate, GM->GameHourDurationSeconds, true, GM->GameHourDurationSeconds);
}

void UGS_GameInstance::OnNewGameLoaded()
{
	AGossipGameMode* GM = Cast<AGossipGameMode>(GetWorld()->GetAuthGameMode());
	RealGameTimeSeconds = 0;
	EraseSaveGame();
	FadeScreen(3, false);
	FTimerDelegate AutoSaveDelegate;
	FString AutoSaveName = "AutoSave";
	AutoSaveDelegate.BindUFunction(this, "SaveGame", AutoSaveName);
	GetWorld()->GetTimerManager().SetTimer(AutoSaveTimerHandle, AutoSaveDelegate, GM->GameHourDurationSeconds, true, GM->GameHourDurationSeconds);
}

void UGS_GameInstance::EraseSaveGame()
{
	TArray<uint8>OutSaveData;
	if (UGameplayStatics::LoadDataFromSlot(OutSaveData, "SaveGame", 0))
	{
		if (UGS_SaveGame_Object* CurrentSaveGame = Cast<UGS_SaveGame_Object>(UGameplayStatics::LoadGameFromMemory(OutSaveData)))
		{
			UGameplayStatics::DeleteGameInSlot("SaveGame", 0);
		}
	}
}

