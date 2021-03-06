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
	ANonPlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGuid Id;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USocialComponent* SocialComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInstinctsComponent* InstinctsComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UFamilyComponent* FamilyComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USaveableEntity* SaveComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	TMap<ECharacterProfile, FCharactersData>CharactersData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UDataTable* NamesDatatable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FCharacterName CharacterName;

	UPROPERTY(EditAnywhere)
	float WalkSpeed = 200;
	UPROPERTY(EditAnywhere)
	float FastWalkSpeed = 300;
	UPROPERTY(EditAnywhere)
	float RunSpeed = 500;

	UFUNCTION(BlueprintCallable)
	bool GetSelected() const { return bSelected; }

	virtual void Tick(float DeltaTime) override;

	void SetCharacterProfile(AActor* ShelterActor);
	ECharacterProfile GetCharacterGender();

	AGS_AIController* GetAIController() { return AIController; }

	void OrderMove(FVector Location);
	void SetMoveSpeed(int32 SpeedLevel);
	void SetSelected(bool Value) { bSelected = Value; }

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveValues)override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	class UCharactersDataAsset* CharactersDataAsset;

	virtual void BeginPlay() override;

	void InitializeCharacterProfile();

	void InitializeCharacterName();

private:
	AGS_AIController* AIController;
	AActor* ActorInVincinity;

	bool bSelected;

	void OnAsyncLoadComplete();

	UFUNCTION()
	void OnAiGoalChanded(int32 SpeedLevel);
	UFUNCTION()
	void OnInstinctsUpdate(TArray<EAIGoal> HungryInstincts);
	UFUNCTION()
	void OnCharacterNameChanged(FCharacterName NewName);
	UFUNCTION()
	void OnMovingShelter(AActor* OtherActor);

	// Example usage GetEnumValueAsString<EVictoryEnum>("EVictoryEnum", VictoryEnum)));
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueAsString(const FString& Name, TEnum Value) {
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr) return FString("Invalid");
		return enumPtr->GetNameByValue((int64)Value).ToString();
	}
};
