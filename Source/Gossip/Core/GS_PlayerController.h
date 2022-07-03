// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GS_PlayerController.generated.h"

USTRUCT(BlueprintType)
struct FCameraOccludedActor
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	const AActor* Actor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStaticMeshComponent*> StaticMeshes;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UMaterialInterface*> Materials;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bOccluded;
};

UCLASS()
class GOSSIP_API AGS_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGS_PlayerController();

	void StartCheckingForActorToOcclude();
	void StopCheckingForActorToOcclude();

	UFUNCTION(BlueprintCallable)
	void SyncOccludedActors();

protected:
	virtual void BeginPlay() override;

	/** How much of the Pawn capsule Radius and Height
	 * should be used for the Line Trace before considering an Actor occluded?
	 * Values too low may make the camera clip through walls.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Occlusion|Occlusion", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float CapsulePercentageForTrace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Occlusion|Materials")
	class UMaterialInterface* FadeMaterial;
	UPROPERTY(BlueprintReadWrite, Category = "Camera Occlusion|Components")
	class USpringArmComponent* ActiveSpringArm;
	UPROPERTY(BlueprintReadWrite, Category = "Camera Occlusion|Components")
	class UCameraComponent* ActiveCamera;
	UPROPERTY(BlueprintReadWrite, Category = "Camera Occlusion|Components")
	class UCapsuleComponent* ActiveCapsuleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Occlusion")
	bool bOcclusionEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Occlusion|Occlusion")
	bool bDebugLineTraces;

private:
	TMap<const AActor*, FCameraOccludedActor> OccludedActors;
	FTimerHandle OcclusionTimerHandle;

	bool HideOccludedActor(const AActor* Actor);
	bool OnHideOccludedActor(const FCameraOccludedActor& OccludedActor) const;
	void ShowOccludedActor(FCameraOccludedActor& OccludedActor);
	bool OnShowOccludedActor(const FCameraOccludedActor& OccludedActor) const;
	void ForceShowOccludedActors();

	__forceinline bool ShouldCheckCameraOcclusion() const
	{
		return bOcclusionEnabled && FadeMaterial && ActiveCamera && ActiveCapsuleComponent;
	}


};
