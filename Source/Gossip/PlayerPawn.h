// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class ANonPlayerCharacter;

UCLASS()
class GOSSIP_API APlayerPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UFloatingPawnMovement* MovementComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


public:
	APlayerPawn();

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void InputYaw(float Value);
	void InputPitch(float Value);
// 	void TurnAtRate(float Rate);
// 	void LookUpAtRate(float Rate);
	void LeftClickPressed();
	void LeftClickReleased();
	void RightClickPressed();
	void RightClickReleased();

	void EscapeMenu();

public:	
	virtual void Tick(float DeltaTime) override;

	FVector GetBoxSelectionStartPosition() { return SelectionBoxStartPosition; }
	FVector GetBoxSelectionEndPosition() { return SelectionBoxEndPosition; }
	bool GetSelectionActive() { return bSelectionActive; }
	void SetCurrentSelections(TArray<ANonPlayerCharacter*> Selections) { CurrentSelections = Selections; }

	FVector2D GetMousePositionAtStart() { return MousePositionAtStart; }
	FVector2D GetMousePosition();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	class APlayerController* PC;
	TArray<ANonPlayerCharacter*>CurrentSelections;

	FVector SelectionBoxStartPosition;
	FVector SelectionBoxEndPosition;
	FVector2D MousePositionAtStart;
	bool bSelectionActive;
	bool bRightClick;

	void StartBoxSelection();
	void EndBoxSelection();

};
