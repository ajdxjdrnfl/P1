// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/WidgetComponent/WidgetComponentBase.h"
#include "P1/Data/SkillData.h"
#include "CharacterWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UCharacterWidgetComponent : public UWidgetComponentBase
{
	GENERATED_BODY()

public:
	friend class AP1Character;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = Editable, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> CharacterOverlayWidgetClass;

	UPROPERTY()
	class UCharacterOverlayWidget* CharacterOverlayWidget;

	UPROPERTY(EditAnywhere, Category = Editable, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> SkillGaugeWidgetClass;

	UPROPERTY()
	class UCastingSkillWidget* SkillGaugeWidget;

	UPROPERTY()
	class AP1Character* OwnerCharacter;

	void OpenOverlayWidget();

public:
	virtual void InitOnSpawn(class UStatComponentBase* StatComponent) override;
	virtual void UseSkill(uint32 SkillIndex) override;
	void SetCharacterStat(class UCharacterStatComponent* StatComponent);
	
	UFUNCTION()
	void OnCharacterStaminaChanged();
	UFUNCTION()
	void OnCharacterHealthChanged();

	void OpenSkillGaugeWidget(float CastingTime);
	void CloseSkillGaugeWidget();
	float GetGaugeRate();
	void SetSkillButton();
	virtual void SpawnDamageIndicator(float Damage) override;
	
	FORCEINLINE class UCharacterOverlayWidget* GetCharacterOverlayWidget() const { return CharacterOverlayWidget; }
	FORCEINLINE FSkillInfo GetSkillInfoBySkillIndex(int32 SkillIndex) const { return Skills[SkillIndex]; }

};
