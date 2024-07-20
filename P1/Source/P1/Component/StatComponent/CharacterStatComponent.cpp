// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatComponent.h"

UCharacterStatComponent::UCharacterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCharacterStatComponent::TakeDamage(FDamageInfo DamageInfo)
{
	// MainCharacter->GetPlayerCombatComponent()->ReceiveAttack(DamageInfo);

	Health -= DamageInfo.Damage;

	OnCharacterHealthChangedDelegate.Broadcast();

	if (Health <= 0)
	{
		//MainCharacter->Die();
	}
}

void UCharacterStatComponent::UseStamina(float Amount)
{
	Stamina -= Amount;
	if (Stamina < 0) Stamina = 0;

	// OwnerCharacter->GetWidgetManagerComponent()->CharacterOverlayWidget->SetStaminaBar();
}

void UCharacterStatComponent::InitStat()
{
	Health = MaxHealth;
	Stamina = MaxStamina;
}
