// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NonPlayerCharacter.generated.h"

class AGS_AIController;
class ARessource;

UCLASS()
class GOSSIP_API ANonPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANonPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UAlignmentComponent* AlignmentComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UInstinctsComponent* InstinctsComp;

	UPROPERTY(EditAnywhere)
	float WalkSpeed = 200;
	UPROPERTY(EditAnywhere)
	float RunSpeed = 500;

	void OrderMove(FVector Location);
	void SetMoveSpeed(bool bRunning);

	virtual void Tick(float DeltaTime) override;

	AGS_AIController* GetAIController() { return AIController; }

	void SetSelected(bool Value) { bSelected = Value; }

protected:
	virtual void BeginPlay() override;

private:
	AGS_AIController* AIController;
	AActor* ActorInVincinity;

	bool bSelected;

	UFUNCTION()
	void OnAiGoalChanded(bool bRun);

};