// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillManagerSubSystem.h"
#include "P1/P1GameInstance.h"
#include "P1/Skill/SkillActorBase.h"

void USkillManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

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
