// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDCanvas.h"
#include "Engine/Canvas.h"
#include "Gossip/PlayerPawn.h"
#include "Gossip/NonPlayerCharacter.h"



void AHUDCanvas::DrawHUD()
{
	Super::DrawHUD();

	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		UIScale = ViewportSize.X / 2048.f;
	}
	if (GetPlayer() && GetPlayer()->GetSelectionActive())
	{
		const FVector2D SelectionStartPosition = GetPlayer()->GetMousePositionAtStart();
		DrawSelectionGrid(SelectionStartPosition);
	}
}

void AHUDCanvas::DrawSelectionGrid(FVector2D GridStartPos)
{
	FVector2D MousePosition = GetPlayer()->GetMousePosition();
	FVector2D EndPosition = MousePosition - GridStartPos;

// 	DrawRect(FLinearColor(1, 1, 1, 0.1), GridStartPos.X, GridStartPos.Y, EndPosition.X, EndPosition.Y);
// 
//  	UE_LOG(LogTemp, Warning, TEXT("X: %s - Y: %s"), *FString::SanitizeFloat(EndPosition.X), *FString::SanitizeFloat(EndPosition.Y))
	

	FCanvasTileItem FillTileItem(GridStartPos, EndPosition, FLinearColor(1, 1, 1, 0.2));
	FillTileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(FillTileItem);

	FCanvasTileItem TileItem(GridStartPos, FVector2D(EndPosition.X, 1), FLinearColor(1, 1, 1, 1));
	FillTileItem.BlendMode = SE_BLEND_Translucent;
	TileItem.UV1 = FVector2D(EndPosition.X * 0.1, 1);
	Canvas->DrawItem(TileItem);

	TileItem.Position = GridStartPos + FVector2D(0, EndPosition.Y);
	Canvas->DrawItem(TileItem);

	TileItem.Position = GridStartPos;
	TileItem.Size = FVector2D(1, EndPosition.Y);
	TileItem.UV1 = FVector2D(1, EndPosition.Y * 0.1);
	Canvas->DrawItem(TileItem);

	TileItem.Position = GridStartPos + FVector2D(EndPosition.X, 0);
	Canvas->DrawItem(TileItem);

	TArray<ANonPlayerCharacter*> SelectedActors;
	GetActorsInSelectionRectangle(GridStartPos, MousePosition, SelectedActors, false, false);
	GetPlayer()->SetCurrentSelections(SelectedActors);
}

APlayerPawn* AHUDCanvas::GetPlayer() const
{
	return Cast<APlayerPawn>(PlayerOwner->GetPawn());
}
