// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillManagerSubSystem.h"
#include "P1/P1GameInstance.h"
#include "P1/Skill/SkillActorBase.h"

void USkillManagerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	for (auto Info : Cast<UP1GameInstance>(GetGameInstance())->SkillInfo)
	{
		SkillCanUseMap.Add(Info.Key, true);
	}
}
