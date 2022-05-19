// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDCanvas.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API AHUDCanvas : public AHUD
{
	GENERATED_BODY()

private:
	// Main drawing function
	virtual void DrawHUD() override;
	class APlayerPawn* GetPlayer() const;

	void DrawSelectionGrid(FVector2D GridStartPos);

	float UIScale;

};
