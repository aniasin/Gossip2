// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gossip/Interfaces/HandleRaycast.h"
#include "GameFramework/Character.h"
#include "NonPlayerCharacter.generated.h"

UCLASS()
class GOSSIP_API ANonPlayerCharacter : public ACharacter, public IHandleRaycast
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANonPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USphereComponent* RedCircle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UAlignmentComponent* AlignmentComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	AActor* ActorInVincinity;

	// HandleRaycast Interface
	virtual bool HandleRaycast(APlayerController* PlayerController) override;
	virtual void SetSelected(bool Value) override { bSelected = Value; }
	bool bRaycastHandled;
	bool bSelected;

};
