// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuBase.h"

#include "Gossip/Data/DataLibrary.h"
#include "MenuSocialRules.generated.h"

/**
 * 
 */
UCLASS()
class GOSSIP_API UMenuSocialRules : public UMenuBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* Switcher;
	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Back;
	UPROPERTY(meta = (BindWidget))
	class UButton* BTN_Confirm;
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ComboBox_Family;
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ComboBox_Wedding;

	UFUNCTION(BlueprintImplementableEvent)
	void ButtonBack_Event();
	UFUNCTION(BlueprintImplementableEvent)
	void ButtonConfirm_Event();

	virtual void OpenMenu()override;

protected:
	virtual bool Initialize();

	UPROPERTY(BlueprintReadWrite)
	bool bOpen;


private:
	UFUNCTION()
	void ResponseBack();
	UFUNCTION()
	void ResponseConfirm();
	UFUNCTION()
	void ComboKeyFamilyChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void ComboKeyWeddingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	FWeddingRule WeddingRule;
	TMap<FString, EFamilySystem>FamilySystem;
	TMap<FString, EWeddingSystem>WeddingSystem;

};
