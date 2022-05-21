// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InstinctsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOSSIP_API UInstinctsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInstinctsComponent();


	UPROPERTY(EditAnywhere, Category = "Instincts") //TODO EditAnywhere for test purpose
	TMap<uint8, float> BasicInstincts;

protected:
	virtual void BeginPlay() override;

private:
		
};
