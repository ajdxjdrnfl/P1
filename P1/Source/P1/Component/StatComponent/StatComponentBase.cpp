// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponentBase.h"
#include "P1/P1Creature.h"
#include "P1/P1.h"

UStatComponentBase::UStatComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UStatComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UStatComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatComponentBase::TakeDamage()
{

}

float UStatComponentBase::GetCurrentHealth()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return 0;

	return Creature->ObjectInfo->hp();
}

float UStatComponentBase::GetMaxHealth()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return 0;

	return Creature->ObjectInfo->max_hp();
}

void UStatComponentBase::SetCurrentHealth(float HealthToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->ObjectInfo->set_hp(HealthToSet);

	if (Creature->ObjectInfo->hp() <= 0)
	{
		//OwnerCreature->
	}
}

void UStatComponentBase::SetMaxHealth(float HealthToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->ObjectInfo->set_max_hp(HealthToSet);
}

