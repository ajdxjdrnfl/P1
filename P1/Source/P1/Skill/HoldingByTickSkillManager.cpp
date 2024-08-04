// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldingByTickSkillManager.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillInstanceBase.h"

void AHoldingByTickSkillManager::StartCasting(float CastingTime)
{
	Super::StartCasting(CastingTime);

	SkillInstance->SpawnSkill();
}
