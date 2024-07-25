// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatComponent.h"
#include "P1/P1.h"
#include "P1/P1Creature.h"

void UEnemyStatComponent::InitStat()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->ObjectInfo->set_hp(Creature->ObjectInfo->hp());

	OnEnemyHealthChangedDelegate.Broadcast();
}

void UEnemyStatComponent::InitStat(float InitHealth)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	//Creature->Info->set_hp(InitHealth);

	OnEnemyHealthChangedDelegate.Broadcast();
}

void UEnemyStatComponent::TakeDamage()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	//Health -= DamageInfo.Damage;
	/*if (Health < 0) Health = 0;

	OnEnemyHealthChangedDelegate.Broadcast();

	if (Health <= 0)
	{
		//MainCharacter->Die();
	}*/
}

void UEnemyStatComponent::SetHealth(float HealthToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->ObjectInfo->set_hp(HealthToSet);

	OnEnemyHealthChangedDelegate.Broadcast();
}
