// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_GameInstance.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	FString LogMessage;
	OnlineSubsystem != nullptr ? LogMessage = OnlineSubsystem->GetSubsystemName().ToString() : "OnlineSubsystem not found!";
	UE_LOG(LogTemp, Warning, TEXT("Loaded subsystem: %s"), *LogMessage)

	GEngine->OnNetworkFailure().AddUObject(this, &UGS_GameInstance::NetworkError);
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
	SocialRulesMenu = CreateWidget<UMenuBase>(this, SocialRulesMenuClass);
	SocialRulesMenu->SetMenuInterface(this);
	SocialRulesMenu->SetUp(false);
}

void UGS_GameInstance::OpenSocialRuleMenu()
{
	if (!SocialRulesMenu) return;
	SocialRulesMenu->OpenMenu();
}

void UGS_GameInstance::NewGame()
{
	if (Menu) Menu->TearDown();
	FLatentActionInfo LatentInfo;
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
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Main Menu... "));
	GetFirstLocalPlayerController()->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UGS_GameInstance::QuitGame()
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
		}
	}
	return SaveData;
}

bool UGS_GameInstance::CreateSaveGameBinary(TMap<FGuid, FSaveStruct>SaveData)
{
	UGS_SaveGame_Object* CurrentSaveGame = Cast<UGS_SaveGame_Object>(UGameplayStatics::CreateSaveGameObject(UGS_SaveGame_Object::StaticClass()));
	if (!CurrentSaveGame) return false;
	CurrentSaveGame->SaveData = SaveData;

	TArray<uint8>OutSaveData;
	if (!UGameplayStatics::SaveGameToMemory(CurrentSaveGame, OutSaveData)) return false;
	
	return UGameplayStatics::SaveDataToSlot(OutSaveData, "SaveGame", 0);
}

void UGS_GameInstance::RestoreGameState()
{
	Travel("Map_01");
}

void UGS_GameInstance::AsyncLoadGame()
{
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	LoadedDelegate.BindUObject(this, &UGS_GameInstance::OnFinishedLoadGameData);
	UGameplayStatics::AsyncLoadGameFromSlot("SaveGame", 0, LoadedDelegate);
}

void UGS_GameInstance::OnFinishedLoadGameData(const FString& SaveName, const int32 Index, USaveGame* GameObject)
{
	UGS_SaveGame_Object* SaveGameObject = Cast<UGS_SaveGame_Object>(GameObject);
	if (!SaveGameObject) return;

	UE_LOG(LogTemp, Warning, TEXT("Loading Game..."))

	TMap<FGuid, FSaveStruct>SaveData = SaveGameObject->SaveData;

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

void UGS_GameInstance::OnMapLoaded()
{
	AsyncLoadGame();
}
