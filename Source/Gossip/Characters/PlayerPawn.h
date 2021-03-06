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

	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* Capsule;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USocialRulesComponent* SocialRulesComponent;

public:
	APlayerPawn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGuid Id = FGuid(1, 3, 3, 7);

	virtual void Tick(float DeltaTime) override;

	void ActorHeightLevel();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MoveToLocation();
	void InputYaw(float Value);
	void InputPitch(float Value);
	void CameraZoom(float Value);
	void LeftClickPressed();
	void LeftClickReleased();
	void RightClickPressed();
	void RightClickReleased();
	void EscapeMenu();
	void SaveGame();
	void LoadGame();

private:
	class APlayerController* PC;

	bool bMoving;

	void DebugInfo();

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}

};
