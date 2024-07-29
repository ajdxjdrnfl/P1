// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillManagerSubSystem.h"
#include "P1/P1GameInstance.h"
#include "P1/Skill/SkillActorBase.h"

void USkillManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SkillCanUseMap.Add(0, true);
	SkillCanUseMap.Add(1, true);
	SkillCanUseMap.Add(2, true);
	SkillCanUseMap.Add(3, true);
}
