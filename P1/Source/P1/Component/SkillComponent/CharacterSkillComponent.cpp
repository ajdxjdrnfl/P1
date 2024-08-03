// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkillComponent.h"
#include "Engine/DataTable.h"
#include "P1/P1Character.h"

void UCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SetSkills();
}

void UCharacterSkillComponent::UseSkill(int32 SkillIndex)
{
	Super::UseSkill(SkillIndex);
}
