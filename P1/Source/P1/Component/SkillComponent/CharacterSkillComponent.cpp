// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkillComponent.h"
#include "Engine/DataTable.h"

void UCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SetSkills();
}

void UCharacterSkillComponent::UseSkill(uint16 SkillIndex)
{
	Super::UseSkill(SkillIndex);
}

