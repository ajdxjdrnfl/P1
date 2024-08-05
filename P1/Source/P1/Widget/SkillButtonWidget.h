// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P1/Data/SkillData.h"
#include "SkillButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API USkillButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	uint16 SkillSlotNum = 4;

	UPROPERTY(meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UWrapBox* SkillButtons;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> SkillSlotWidgetClass;

	UPROPERTY(EditAnywhere)
	TArray<FString> KeyString;

public:
	void UseSkill(FSkillInfo SkillInfo);
	void SetSkillButton(TArray<FSkillInfo> SkillArray);
};
