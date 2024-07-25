// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatComponent.h"
#include "P1/P1Character.h"
#include "P1/P1.h"
#include "P1/P1Creature.h"

UCharacterStatComponent::UCharacterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCharacterStatComponent::TakeDamage()
{
	//Health -= DamageInfo.Damage;
	//if (Health < 0) Health = 0;
	//
	//OnCharacterHealthChangedDelegate.Broadcast();
//
	//if (Health <= 0)
	//{
		//MainCharacter->Die();
	//}
}

void UCharacterStatComponent::UseStamina(float Amount)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	//Creature->Info->stamina() -= Amount;
	//
	//if (Stamina < 0) Stamina = 0;

	//OnCharacterStaminaChangedDelegate.Broadcast();
}

void UCharacterStatComponent::InitStat()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->ObjectInfo->set_hp(Creature->ObjectInfo->max_hp());
	Creature->ObjectInfo->set_stamina(Creature->ObjectInfo->max_stamina());

	OnCharacterHealthChangedDelegate.Broadcast();
	OnCharacterStaminaChangedDelegate.Broadcast();
}

void UCharacterStatComponent::InitStat(float HealthToSet, float StaminaToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	//Creature->Info->set_hp(HealthToSet);
	//Creature->Info->set_stamina(StaminaToSet);

	OnCharacterHealthChangedDelegate.Broadcast();
	OnCharacterStaminaChangedDelegate.Broadcast();
}

void UCharacterStatComponent::SetCurrentStamina(float StaminaToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->ObjectInfo->set_stamina(StaminaToSet);
}

float UCharacterStatComponent::GetCurrentStamina()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return 0;

	return Creature->ObjectInfo->stamina();
}

void UCharacterStatComponent::SetMaxStamina(float StaminaToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->ObjectInfo->set_max_stamina(StaminaToSet);
}

float UCharacterStatComponent::GetMaxStamina()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return 0;

	return Creature->ObjectInfo->max_stamina();
}
