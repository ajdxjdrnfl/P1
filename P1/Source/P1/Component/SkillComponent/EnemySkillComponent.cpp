// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkillComponent.h"
#include "P1/Skill/Boss/BossSkillInstance.h"

void UEnemySkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemySkillComponent::UseSkill(int32 SkillIndex)
{
	Super::UseSkill(SkillIndex);
}

void UEnemySkillComponent::UsePillarSkill(FVector2D Location)
{
	if (GetSkillInstances().Num() <= 1 || GetSkillInstances()[1] == nullptr) return;

	GetSkillInstances()[1]->SpawnSkillAtLocation(Location);
}
