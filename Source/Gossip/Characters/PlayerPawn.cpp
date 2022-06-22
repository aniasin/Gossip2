// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerOrdersComponent.h"
#include "SocialRulesComponent.h"

#include "Gossip/Save/SaveableEntity.h"
#include "Gossip/Core/GS_GameInstance.h"


// Sets default values
APlayerPawn::APlayerPawn()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	RootComponent = CameraBoom;
	//CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));

	SaveComponent = CreateDefaultSubobject<USaveableEntity>(TEXT("SaveComp"));
	SaveComponent->Id = Id;
	PlayerOrdersComponent = CreateDefaultSubobject<UPlayerOrdersComponent>(TEXT("PlayerOrdersComp"));
	SocialRulesComponent = CreateDefaultSubobject<USocialRulesComponent>(TEXT("SocialRulesComp"));
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APlayerPawn::InputYaw);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APlayerPawn::InputPitch);

	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &APlayerPawn::EscapeMenu);
	PlayerInputComponent->BindAction("Save", IE_Pressed, this, &APlayerPawn::SaveGame);
	PlayerInputComponent->BindAction("Load", IE_Pressed, this, &APlayerPawn::LoadGame);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &APlayerPawn::LeftClickPressed);
	PlayerInputComponent->BindAction("LeftClick", IE_Released, this, &APlayerPawn::LeftClickReleased);
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &APlayerPawn::RightClickPressed);
	PlayerInputComponent->BindAction("RightClick", IE_Released, this, &APlayerPawn::RightClickReleased);

}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		PC->SetShowMouseCursor(true);
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PlayerOrdersComponent->PlayerController = PC;
	}
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Input
void APlayerPawn::MoveForward(float Value)
{
	if (PlayerOrdersComponent->GetPlayerIsMoved()) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerPawn::MoveRight(float Value)
{
	if (PlayerOrdersComponent->GetPlayerIsMoved()) return;
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void APlayerPawn::InputYaw(float Value)
{
	if (PlayerOrdersComponent->GetPlayerIsMoved()) return;
	AddControllerYawInput(Value);
}

void APlayerPawn::InputPitch(float Value)
{
	if (PlayerOrdersComponent->GetPlayerIsMoved()) return;
}

void APlayerPawn::LeftClickPressed()
{
	PlayerOrdersComponent->bControlKeyDown = (PC->IsInputKeyDown(EKeys::LeftControl));
	PlayerOrdersComponent->StartBoxSelection();
}

void APlayerPawn::LeftClickReleased()
{
	PlayerOrdersComponent->bControlKeyDown = false;
	PlayerOrdersComponent->EndBoxSelection();
}

void APlayerPawn::RightClickPressed()
{
	PlayerOrdersComponent->OrderMoveUnderCursor();
}

void APlayerPawn::RightClickReleased()
{

}

void APlayerPawn::EscapeMenu()
{
	UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetGameInstance());
	if (!GI) return;
	GI->LoadGameMenu();
}

void APlayerPawn::SaveGame()
{
	UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetGameInstance());
	if (!GI) return;
	GI->SaveGame();
}

void APlayerPawn::LoadGame()
{
	UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetGameInstance());
	if (!GI) return;
	GI->RestoreGameState();
}

