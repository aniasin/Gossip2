// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NonPlayerCharacter.generated.h"

class AGS_AIController;

UCLASS()
class GOSSIP_API ANonPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANonPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UAlignmentComponent* AlignmentComp;

	void OrderMove(FVector Location);
	void SetMoveSpeed(bool bRunning);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AGS_AIController* GetAIController() { return AIController; }

	void SetSelected(bool Value) { bSelected = Value; }

private:
	AGS_AIController* AIController;
	AActor* ActorInVincinity;

	bool bSelected;

	UFUNCTION()
	void OnAiGoalChanded(bool bRun);

};
