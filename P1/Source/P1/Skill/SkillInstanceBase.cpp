// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillInstanceBase.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"

ASkillInstanceBase::ASkillInstanceBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
}

void ASkillInstanceBase::Init(AP1Character* _OwnerCharacter)
{
	OwnerCharacter = _OwnerCharacter;

	check(OwnerCharacter != nullptr);
}

void ASkillInstanceBase::DespawnSkill()
{
	if (SpawnedSkillActor)
	{
		SpawnedSkillActor->Destroy();
	}
}
