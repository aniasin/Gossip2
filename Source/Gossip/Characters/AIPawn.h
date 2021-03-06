// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Gossip/Data/DataLibrary.h"

#include "AIPawn.generated.h"

UCLASS()
class GOSSIP_API AAIPawn : public APawn
{
	GENERATED_BODY()

public:
	AAIPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USaveableEntity* SaveComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USocialRulesComponent* SocialRulesComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGuid Id;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeddingRule WeddingRules;

protected:

	virtual void BeginPlay() override;

private:	


};
