// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatComponent.h"
#include "P1/P1.h"
#include "P1/Enemy/EnemyBase.h"
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
	if (OwnerEnemy == nullptr)
		return;

	OwnerEnemy->ObjectInfo->set_hp(InitHealth);

	OnEnemyHealthChangedDelegate.Broadcast();
}

void UEnemyStatComponent::TakeDamage()
{
	if (OwnerEnemy == nullptr)
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
	if (OwnerEnemy == nullptr)
		return;

	OwnerEnemy->ObjectInfo->set_hp(HealthToSet);

	OnEnemyHealthChangedDelegate.Broadcast();

	if (OwnerEnemy->ObjectInfo->hp() <= 0)
	{
		OwnerEnemy->Die();
	}
}

void UEnemyStatComponent::AllStop()
{

}
