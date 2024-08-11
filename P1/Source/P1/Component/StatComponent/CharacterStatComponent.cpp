// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatComponent.h"
#include "P1/P1Character.h"
#include "P1/P1.h"
#include "P1/P1Creature.h"
#include "P1/Component/WidgetComponent/CharacterWidgetComponent.h"

UCharacterStatComponent::UCharacterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCharacterStatComponent::InitOnSpawn(Protocol::ObjectInfo& Info)
{
	Super::InitOnSpawn(Info);
	InitStat(Info.hp(), Info.stamina());
}

void UCharacterStatComponent::InitStat(float HealthToSet, float StaminaToSet)
{
	if (OwnerCharacter == nullptr)
		return;

	OwnerCharacter->ObjectInfo->set_hp(HealthToSet);
	OwnerCharacter->ObjectInfo->set_stamina(StaminaToSet);
	OwnerCharacter->ObjectInfo->set_max_hp(HealthToSet);
	OwnerCharacter->ObjectInfo->set_max_stamina(HealthToSet);

	OnCharacterHealthChangedDelegate.Broadcast();
	OnCharacterStaminaChangedDelegate.Broadcast();
}

void UCharacterStatComponent::SetCurrentStamina(float StaminaToSet)
{
	if (OwnerCharacter == nullptr)
		return;

	OwnerCharacter->ObjectInfo->set_stamina(StaminaToSet);
}

float UCharacterStatComponent::GetCurrentStamina()
{
	if (OwnerCharacter == nullptr)
		return 0;

	return OwnerCharacter->ObjectInfo->stamina();
}

void UCharacterStatComponent::SetMaxStamina(float StaminaToSet)
{
	if (OwnerCharacter == nullptr)
		return;

	OwnerCharacter->ObjectInfo->set_max_stamina(StaminaToSet);
}

float UCharacterStatComponent::GetMaxStamina()
{
	if (OwnerCharacter == nullptr)
		return 0;

	return OwnerCharacter->ObjectInfo->max_stamina();
}

void UCharacterStatComponent::SetHealth(float HealthToSet)
{
	if (OwnerCharacter == nullptr)
		return;

	OnCharacterHealthChangedDelegate.AddUniqueDynamic(OwnerCharacter->GetWidgetComponent(), &UCharacterWidgetComponent::OnCharacterHealthChanged);

	OwnerCharacter->ObjectInfo->set_hp(HealthToSet);

	OnCharacterHealthChangedDelegate.Broadcast();
}
