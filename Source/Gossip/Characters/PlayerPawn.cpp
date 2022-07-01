// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerOrdersComponent.h"
#include "SocialRulesComponent.h"

#include "NonPlayerCharacter.h"
#include "Gossip/AI/InstinctsComponent.h"
#include "Gossip/Save/SaveableEntity.h"
#include "Gossip/Core/GS_GameInstance.h"


// Sets default values
APlayerPawn::APlayerPawn()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(Capsule);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(Capsule);
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
	PlayerInputComponent->BindAxis("CameraZoom", this, &APlayerPawn::CameraZoom);

	PlayerInputComponent->BindAction("DebugInfo", IE_Pressed, this, &APlayerPawn::DebugInfo);
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

	ActorHeightLevel();

}

void APlayerPawn::ActorHeightLevel()
{
	if (MovementComp->Velocity.X < 5 && MovementComp->Velocity.Y < 5) return;
	
	FHitResult HitResult;
	FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
	FVector End = GetActorUpVector() * - 500;
	FCollisionQueryParams Parameters;
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorUpVector() * -1000 + GetActorLocation(), FColor::Green, false, 1, 0, 5);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel3, Parameters))

	{
		if (GetActorLocation().Z - HitResult.Location.Z < 200)
		{
			MovementComp->AddInputVector(GetActorUpVector(), 1);
		}
		else if (GetActorLocation().Z - HitResult.Location.Z > 200)
		{
			MovementComp->AddInputVector(GetActorUpVector() * -1, 1);
		}
	}	
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

void APlayerPawn::CameraZoom(float Value)
{
	if (PlayerOrdersComponent->GetPlayerIsMoved()) return;
	float NewLength = FMath::Clamp(CameraBoom->TargetArmLength - Value * 100, 0, 3000);
	CameraBoom->SocketOffset = FVector(CameraBoom->SocketOffset.X, CameraBoom->SocketOffset.Y, NewLength);
	CameraBoom->TargetArmLength = NewLength;

	MovementComp->MaxSpeed = FMath::Clamp(NewLength *2, 1200, 6000);
	MovementComp->Acceleration = FMath::Clamp(NewLength * 4, 4000, 24000);
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
	GI->SaveGame("SaveGame");
}

void APlayerPawn::LoadGame()
{
	UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetGameInstance());
	if (!GI) return;
	GI->RestoreGameState("SaveGame");
}

void APlayerPawn::DebugInfo()
{
	TArray<ANonPlayerCharacter*> CurrentSelections = PlayerOrdersComponent->GetCurrentSelections();
	if (CurrentSelections.IsEmpty()) return;

	UE_LOG(LogTemp, Log, TEXT("=== INSTINCTS VALUES ==="));
	UE_LOG(LogTemp, Log, TEXT("=== %s %s ==="), *CurrentSelections[0]->CharacterName.FirstName, *CurrentSelections[0]->CharacterName.LastName);

	TArray<FInstinctValues>Instincts = CurrentSelections[0]->InstinctsComp->InstinctsInfo;
	for (FInstinctValues Instinct : Instincts)
	{
		float CurrentValue = Instinct.CurrentValue;
		float GrowCoef = Instinct.GrowCoeffient;
		float ReportedValue = Instinct.ReportedValue;
		FString InstinctName = GetEnumValueAsString("EAIGoal", Instinct.Goal);
		UE_LOG(LogTemp, Log, TEXT("     "));
		UE_LOG(LogTemp, Log, TEXT("%s"), *InstinctName);
		UE_LOG(LogTemp, Log, TEXT("CurrentValue: %s"), *FString::SanitizeFloat(CurrentValue));
		UE_LOG(LogTemp, Log, TEXT("GrowValue: %s"), *FString::SanitizeFloat(GrowCoef));
		UE_LOG(LogTemp, Log, TEXT("Reported: %s"), *FString::SanitizeFloat(ReportedValue));
		UE_LOG(LogTemp, Log, TEXT("------------"));
	}
}

