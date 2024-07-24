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

void UStatComponentBase::TakeDamage(FDamageInfo DamageInfo)
{

}

float UStatComponentBase::GetCurrentHealth()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return 0;

	return Creature->Info->hp();
}

float UStatComponentBase::GetMaxHealth()
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return 0;

	return Creature->Info->max_hp();
}

void UStatComponentBase::SetCurrentHealth(float HealthToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->Info->set_hp(HealthToSet);
}

void UStatComponentBase::SetMaxHealth(float HealthToSet)
{
	AP1Creature* Creature = Cast<AP1Creature>(GetOwner());
	if (Creature == nullptr)
		return;

	Creature->Info->set_max_hp(HealthToSet);
}

