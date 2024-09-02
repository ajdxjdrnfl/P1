// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillManagerSubSystem.h"
#include "P1/P1GameInstance.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/Skill/Targeting/SkillTargetingBase.h"

void USkillManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CastingSkillGaugeRate = 0;

	bCanMove = true;
	bCanUseSkill = true;
	bCanMoveByAnimMontage = true;

	CurrentSkillTargeting = nullptr;

	SkillCanUseMapByCooldownTime.Empty();
	SkillCanUseMapByCasting.Empty();

	for (int i = 0; i < 4; i++)
	{
		SkillCanUseMapByCooldownTime.Add(i, true);
		SkillCanUseMapByCasting.Add(i, true);
	}
}

void USkillManagerSubSystem::SetKeyCanUse(int32 SkillIndex = -1)
{
	if (SkillIndex == -1)
	{
		for (TPair<uint16, bool>& itr : SkillCanUseMapByCasting)
		{
			itr.Value = true;
		}
		return;
	}

	for (TPair<uint16, bool>& itr : SkillCanUseMapByCasting)
	{
		if (itr.Key == SkillIndex) continue;
		itr.Value = false;
	}
}

bool USkillManagerSubSystem::CanUseSkill(int32 SkillIndex)
{
	if (!bCanUseSkill) return false;
	return SkillCanUseMapByCooldownTime[SkillIndex] && SkillCanUseMapByCasting[SkillIndex];
}

bool USkillManagerSubSystem::CanMove()
{
	return bCanMove && bCanMoveByAnimMontage;
}

void USkillManagerSubSystem::CheckSkillTargeting()
{
	if (IsValid(CurrentSkillTargeting))
	{
		CurrentSkillTargeting->Destroy();
		CurrentSkillTargeting = nullptr;
	}
}

void USkillManagerSubSystem::SetCanMove(bool _bCanMove)
{
	bCanMove = bCanMove;
	bCanUseSkill = bCanMove;
}
