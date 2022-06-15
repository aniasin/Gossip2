// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gossip/Save/SaveGameInterface.h"
#include "PlayerOrdersComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UPlayerOrdersComponent : public UActorComponent, public ISaveGameInterface
{
	GENERATED_BODY()

public:	
	UPlayerOrdersComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ISaveGameInterface
	virtual FSaveValues CaptureState()override;
	virtual void RestoreState(FSaveValues SaveData)override;

	class APlayerController* PlayerController;

	void StartBoxSelection();
	void EndBoxSelection();
	FVector GetBoxSelectionStartPosition() { return SelectionBoxStartPosition; }
	FVector GetBoxSelectionEndPosition() { return SelectionBoxEndPosition; }
	bool GetSelectionActive() { return bSelectionActive; }
	void SetCurrentSelections(TArray<ANonPlayerCharacter*> Selections) { CurrentSelections = Selections; }
	FVector2D GetMousePositionAtStart() { return MousePositionAtStart; }
	FVector2D GetMousePosition();

	void OrderMoveUnderCursor();

protected:
	virtual void BeginPlay() override;

private:	
	TArray<ANonPlayerCharacter*>CurrentSelections;
	FVector SelectionBoxStartPosition;
	FVector SelectionBoxEndPosition;
	FVector2D MousePositionAtStart;
	bool bSelectionActive;

	
};
