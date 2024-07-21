// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/StatComponent/StatComponentBase.h"
#include "P1/Data/SkillData.h"
#include "CharacterStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterHealthChangedDelegate);

UCLASS()
class P1_API UCharacterStatComponent : public UStatComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterStatComponent();

	friend class AP1Character;

	FOnCharacterHealthChangedDelegate OnCharacterHealthChangedDelegate;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class AP1Character* OwnerCharacter;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float Stamina;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxStamina;

public:
	void TakeDamage(/* TODO: FDamageInfo DamageInfo*/);

	void UseStamina(float Amount);

	void InitStat();

	FORCEINLINE float GetCurrentHealth() { return Health; }
	FORCEINLINE float GetMaxHealth() { return MaxHealth; }
	FORCEINLINE void SetCurrentHealth(float HealthToSet) { Health = HealthToSet; }
	FORCEINLINE void SetMaxHealth(float HealthToSet) { MaxHealth = HealthToSet; }
	FORCEINLINE void SetCurrentStamina(float StaminaToSet) { Stamina = StaminaToSet; }
	FORCEINLINE float GetCurrentStamina() { return Stamina; }
	FORCEINLINE void SetMaxStamina(float StaminaToSet) { MaxStamina = StaminaToSet; }
	FORCEINLINE float GetMaxStamina() { return MaxStamina; }
};
