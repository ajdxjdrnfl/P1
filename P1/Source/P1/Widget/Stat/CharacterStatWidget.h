// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatWidgetBase.h"
#include "CharacterStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UCharacterStatWidget : public UStatWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UProgressBar* StaminaBar;

	UPROPERTY()
	class UCharacterStatComponent* CharacterStatComponent;

public:
	void SetHealthBar();
	void SetStaminaBar();

	UFUNCTION()
	void OnCharacterHealthChanged();

	void SetCharacterStat(class UCharacterStatComponent* StatComponent);
};
