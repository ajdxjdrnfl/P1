// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API USkillSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	bool bIsActivated;
	float RemainingCooldownTime;
	float DefaultCooldownTime;
	uint16 SlotID;
	int32 SkillNum;

	UPROPERTY(meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UImage* SkillImage;

	UPROPERTY(meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UImage* CooldownImage;

	UPROPERTY(meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UTextBlock* TimeText;

	UPROPERTY(EditAnywhere, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UTextBlock* KeyText;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* CircleCooldownMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* CooldownMatInstance;

	FText FloatToText1DecimalPlace(float value);

public:
	void ActivateSlot(float time);
	void SetCooldownVisibility(bool bIsVisible);
	void SetSkillImage(class UTexture2D* SkillImageToSet);
	void SetKeyText(FString Key);

	FORCEINLINE void SetSkillNum(int32 Num) { SkillNum = Num; }
	FORCEINLINE bool CanUseSkill() const { return !bIsActivated; }
	FORCEINLINE uint16 GetSlotID() const { return SlotID; }
	FORCEINLINE void SetSlotID(uint16 IDToSet) { SlotID = IDToSet; }
};
