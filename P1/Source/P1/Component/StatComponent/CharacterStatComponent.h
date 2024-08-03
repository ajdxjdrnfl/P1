// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/StatComponent/StatComponentBase.h"
#include "P1/Data/SkillData.h"
#include "CharacterStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterStaminaChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterHealthChangedDelegate);

UCLASS()
class P1_API UCharacterStatComponent : public UStatComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterStatComponent();

	friend class AP1Character;

	FOnCharacterStaminaChangedDelegate OnCharacterStaminaChangedDelegate;
	FOnCharacterHealthChangedDelegate OnCharacterHealthChangedDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class AP1Character* OwnerCharacter;

public:
	virtual void TakeDamage() override;

	void UseStamina(float Amount);

	void InitStat();
	void InitStat(float HealthToSet, float StaminaToSet);

	void SetCurrentStamina(float StaminaToSet);
	float GetCurrentStamina();
	void SetMaxStamina(float StaminaToSet);
	float GetMaxStamina();
	void SetHealth(float HealthToSet);
};
