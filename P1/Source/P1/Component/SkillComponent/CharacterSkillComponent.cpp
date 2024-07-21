// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkillComponent.h"
#include "Engine/DataTable.h"

void UCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SetSkills();
}

void UCharacterSkillComponent::SetSkills()
{
	if (SkillDataTable == nullptr) return;
	FString ContextString;
	FSkillsByClass SkillsByClass = *SkillDataTable->FindRow<FSkillsByClass>(/* TODO: */ FName("Warrior"), ContextString);
	Skills = SkillsByClass.SkillInfos;
}

void UCharacterSkillComponent::UseSkill(int32 SkillIndex)
{
	// TODO: Skills[SkillIndex];
}

