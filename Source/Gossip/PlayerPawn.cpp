// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GS_GameInstance.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gossip//Interfaces/HandleRaycast.h"

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
}


void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerPawn::MoveRight);
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APlayerPawn::InputYaw);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APlayerPawn::InputPitch);

	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &APlayerPawn::EscapeMenu);
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
		PC->SetInputMode(InputMode);
	}
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult HitResult;
	if (PC->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult))
	{
		if (IHandleRaycast* iface = Cast<IHandleRaycast>(HitResult.GetActor()))
		{
			iface->HandleRaycast(PC);
		}
	}
	

}

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
	if (!bRightClick) return;
	AddControllerYawInput(Value);
}

void APlayerPawn::InputPitch(float Value)
{
	if (!bRightClick) return;
	AddControllerPitchInput(Value);
}

void APlayerPawn::RightClickPressed()
{
	PC->SetShowMouseCursor(false);
	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);
	bRightClick = true;
}

void APlayerPawn::RightClickReleased()
{
	PC->SetShowMouseCursor(true);
	FInputModeGameAndUI InputMode;
	PC->SetInputMode(InputMode);
	bRightClick = false;
}

void APlayerPawn::EscapeMenu()
{
	UGS_GameInstance* GI = Cast<UGS_GameInstance>(GetGameInstance());
	if (!GI) return;
	GI->LoadGameMenu();
}

