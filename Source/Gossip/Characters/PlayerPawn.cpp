// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NonPlayerCharacter.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PlayerOrdersComponent.h"

#include "Gossip/Save/SaveableEntity.h"
#include "Gossip/AI/GS_AIController.h"
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
	}
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentSelections.Num() > 0)
	{

	}

}

void APlayerPawn::StartBoxSelection()
{
	for (ANonPlayerCharacter* Selected : CurrentSelections)
	{
		Selected->SetSelected(false);
	}
	MousePositionAtStart = GetMousePosition();
	bSelectionActive = true;
}

void APlayerPawn::EndBoxSelection()
{
	bSelectionActive = false;
	for (ANonPlayerCharacter* Selected : CurrentSelections)
	{
		Selected->SetSelected(true);
	}
}

FVector2D APlayerPawn::GetMousePosition()
{
	FVector2D MousePosition;
	PC->GetMousePosition(MousePosition.X, MousePosition.Y);
	return MousePosition;
}

// Input
void APlayerPawn::MoveForward(float Value)
{
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
	AddControllerYawInput(Value);
}

void APlayerPawn::InputPitch(float Value)
{

}

void APlayerPawn::LeftClickPressed()
{
	StartBoxSelection();
}

void APlayerPawn::LeftClickReleased()
{
	EndBoxSelection();
}

void APlayerPawn::RightClickPressed()
{
	FHitResult Hit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	FVector Location = Hit.Location;

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!NavigationSystem) return;
 
	for (ANonPlayerCharacter* Selected : CurrentSelections)
	{
		uint8 AIStatus = Selected->GetAIController()->BlackboardComponent->GetValueAsEnum("AIStatus");
		if (AIStatus == (uint8)EAIStatus::Follow || AIStatus == (uint8)EAIStatus::LeadHome) continue;
		UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(GetWorld(), Selected->GetActorLocation(), Location);
		if (!Path->IsValid()) continue;
		Selected->OrderMove(Location);
	}
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

