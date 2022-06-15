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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USaveableEntity* SaveComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPlayerOrdersComponent* PlayerOrdersComponent;

public:
	APlayerPawn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGuid Id = FGuid(1, 3, 3, 7);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector GetBoxSelectionStartPosition() { return SelectionBoxStartPosition; }
	FVector GetBoxSelectionEndPosition() { return SelectionBoxEndPosition; }
	bool GetSelectionActive() { return bSelectionActive; }
	void SetCurrentSelections(TArray<ANonPlayerCharacter*> Selections) { CurrentSelections = Selections; }
	FVector2D GetMousePositionAtStart() { return MousePositionAtStart; }
	FVector2D GetMousePosition();

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void InputYaw(float Value);
	void InputPitch(float Value);
	void LeftClickPressed();
	void LeftClickReleased();
	void RightClickPressed();
	void RightClickReleased();
	void EscapeMenu();
	void SaveGame();
	void LoadGame();

private:
	class APlayerController* PC;

	TArray<ANonPlayerCharacter*>CurrentSelections;
	FVector SelectionBoxStartPosition;
	FVector SelectionBoxEndPosition;
	FVector2D MousePositionAtStart;
	bool bSelectionActive;

	void StartBoxSelection();
	void EndBoxSelection();

};
