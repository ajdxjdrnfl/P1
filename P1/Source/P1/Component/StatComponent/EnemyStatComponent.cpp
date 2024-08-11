// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatComponent.h"
#include "P1/P1.h"
#include "P1/Enemy/EnemyBase.h"
#include "P1/P1Creature.h"

void UEnemyStatComponent::InitOnSpawn(Protocol::ObjectInfo& Info)
{
	Super::InitOnSpawn(Info);
	InitStat(Info.hp());
}

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

void UEnemyStatComponent::SetHealth(float HealthToSet)
{
	//if (OwnerEnemy == nullptr)
	//	return;

	OwnerEnemy->ObjectInfo->set_hp(HealthToSet);

	OnEnemyHealthChangedDelegate.Broadcast();
}

void UEnemyStatComponent::AllStop()
{

}
