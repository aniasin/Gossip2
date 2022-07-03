// staticMeshes occlusion courtesy of https://alfredbaudisch.com/


#include "GS_PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

AGS_PlayerController::AGS_PlayerController()
{
	CapsulePercentageForTrace = 1.0f;
	bDebugLineTraces = true;
	bOcclusionEnabled = true;
}

void AGS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetPawn())) return;	
	ActiveSpringArm = Cast<USpringArmComponent>(GetPawn()->GetComponentByClass(USpringArmComponent::StaticClass()));
	ActiveCamera = Cast<UCameraComponent>(GetPawn()->GetComponentByClass(UCameraComponent::StaticClass()));
	ActiveCapsuleComponent = Cast<UCapsuleComponent>(GetPawn()->GetComponentByClass(UCapsuleComponent::StaticClass()));	
}

void AGS_PlayerController::StartCheckingForActorToOcclude()
{
	GetWorldTimerManager().SetTimer(OcclusionTimerHandle, this, &AGS_PlayerController::SyncOccludedActors, 0.5, true, 1);
}

void AGS_PlayerController::StopCheckingForActorToOcclude()
{
	GetWorldTimerManager().ClearTimer(OcclusionTimerHandle);
}

void AGS_PlayerController::SyncOccludedActors()
{
	if (!ShouldCheckCameraOcclusion()) return;

	// Camera is currently colliding, show all current occluded actors
	// and do not perform further occlusion
	if (ActiveSpringArm->bDoCollisionTest)
	{
		ForceShowOccludedActors();
		return;
	}

	FVector Start = GetPawn()->GetActorLocation();
	FVector End = Start + ActiveCamera->GetForwardVector() * 1000;

	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes;
	CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("IgnoreOcclusionCamera"), ActorsToIgnore);
	TArray<FHitResult> OutHits;

	auto ShouldDebug = bDebugLineTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bGotHits = UKismetSystemLibrary::CapsuleTraceMultiForObjects( 
		GetWorld(), Start, End, ActiveCapsuleComponent->GetScaledCapsuleRadius() * CapsulePercentageForTrace,
		ActiveCapsuleComponent->GetScaledCapsuleHalfHeight() * CapsulePercentageForTrace, CollisionObjectTypes, true,
		ActorsToIgnore,
		ShouldDebug,
		OutHits, true);

	if (bGotHits)
	{
		// The list of actors hit by the line trace, that means that they are occluded from view
		TSet<const AActor*> ActorsJustOccluded;

		// Hide actors that are occluded by the camera
		for (FHitResult Hit : OutHits)
		{
			const AActor* HitActor = Cast<AActor>(Hit.GetActor());
			HideOccludedActor(HitActor);
			ActorsJustOccluded.Add(HitActor);
		}

		// Show actors that are currently hidden but that are not occluded by the camera anymore 
		for (auto& Elem : OccludedActors)
		{
			if (ActorsJustOccluded.Contains(Elem.Value.Actor) && Elem.Value.bOccluded) continue;
			ShowOccludedActor(Elem.Value);

			if (!bDebugLineTraces) continue;			
			UE_LOG(LogTemp, Warning, TEXT("Actor %s was occluded, but it's not occluded anymore with the new hits."), *Elem.Value.Actor->GetName());			
		}
	}
	else
	{
		ForceShowOccludedActors();
	}
}

bool AGS_PlayerController::HideOccludedActor(const AActor* Actor)
{
	FCameraOccludedActor* ExistingOccludedActor = OccludedActors.Find(Actor);

	if (ExistingOccludedActor && ExistingOccludedActor->bOccluded)
	{
		if (bDebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was already occluded. Ignoring."), *Actor->GetName());
		return false;
	}

	if (ExistingOccludedActor && IsValid(ExistingOccludedActor->Actor))
	{
		ExistingOccludedActor->bOccluded = true;
		OnHideOccludedActor(*ExistingOccludedActor);

		if (bDebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s exists, but was not occluded. Occluding it now."), *Actor->GetName());
	}
	else
	{
		TArray<UStaticMeshComponent*>StaticMeshes;
		Actor->GetComponents<UStaticMeshComponent>(StaticMeshes);

		FCameraOccludedActor OccludedActor;
		OccludedActor.Actor = Actor;
		OccludedActor.StaticMeshes = StaticMeshes;
		for (UStaticMeshComponent* StaticMesh : StaticMeshes)
		{
			if (!IsValid(StaticMesh->GetStaticMesh())) continue;
			OccludedActor.Materials.Append(StaticMesh->GetMaterials());
		}
		OccludedActor.bOccluded = true;
		OccludedActors.Add(Actor, OccludedActor);
		OnHideOccludedActor(OccludedActor);

		if (bDebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s does not exist, creating and occluding it now."), *Actor->GetName());
	}

	return true;
}

void AGS_PlayerController::ForceShowOccludedActors()
{
	for (auto& Elem : OccludedActors)
	{
		if (!Elem.Value.bOccluded) continue;		
		ShowOccludedActor(Elem.Value);
		if (bDebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was occluded, force to show again."), *Elem.Value.Actor->GetName());		
	}
}

void AGS_PlayerController::ShowOccludedActor(FCameraOccludedActor& OccludedActor)
{
	if (!IsValid(OccludedActor.Actor))
	{
		OccludedActors.Remove(OccludedActor.Actor);
	}

	OccludedActor.bOccluded = false;
	OnShowOccludedActor(OccludedActor);
}

bool AGS_PlayerController::OnShowOccludedActor(const FCameraOccludedActor& OccludedActor) const
{
	int32 Index = 0;
	for (UStaticMeshComponent* StaticMesh : OccludedActor.StaticMeshes)
	{
		if (!IsValid(StaticMesh->GetStaticMesh())) continue;
		StaticMesh->SetMaterial(0, OccludedActor.Materials[Index]);
		Index++;
	}
	return true;
}

bool AGS_PlayerController::OnHideOccludedActor(const FCameraOccludedActor& OccludedActor) const
{
	int32 Index = 0;
	for (UStaticMeshComponent* StaticMesh : OccludedActor.StaticMeshes)
	{
		StaticMesh->SetMaterial(0, FadeMaterial);
		Index++;
	}
	return true;
}

