// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Gossip/Data/DataLibrary.h"
#include "Gossip/Save/SaveGameInterface.h"
#include "NonPlayerCharacter.generated.h"

class AGS_AIController;
class ARessource;

UCLASS()
class GOSSIP_API ANonPlayerCharacter : public ACharacter, public ISaveGameInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANonPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USocialComponent* SocialComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInstinctsComponent* InstinctsComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TMap<ECharacterProfile, FCharactersData>CharactersData;

	UPROPERTY(EditAnywhere)
	float WalkSpeed = 200;
	UPROPERTY(EditAnywhere)
	float FastWalkSpeed = 300;
	UPROPERTY(EditAnywhere)
	float RunSpeed = 500;

	void OrderMove(FVector Location);
	void SetMoveSpeed(int32 SpeedLevel);

	virtual void Tick(float DeltaTime) override;

	AGS_AIController* GetAIController() { return AIController; }

	void SetSelected(bool Value) { bSelected = Value; }

	// SaveInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues Values)override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	class UCharactersDataAsset* CharactersDataAsset;

	virtual void BeginPlay() override;

	void InitializeCharacterProfile();

private:
	AGS_AIController* AIController;
	AActor* ActorInVincinity;

	bool bSelected;

	void OnAsyncLoadComplete();

	UFUNCTION()
	void OnAiGoalChanded(int32 SpeedLevel);
	UFUNCTION()
	void OnInstinctsUpdate(TArray<EAIGoal> HungryInstincts);

};
