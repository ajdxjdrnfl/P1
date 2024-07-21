// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlayWidget.h"
#include "SkillButtonWidget.h"
#include "Stat/CharacterStatWidget.h"

void UCharacterOverlayWidget::UseSkill(FSkillInfo SkillInfo)
{
	if (SkillWidget == nullptr)
		return;

	SkillWidget->UseSkill(SkillInfo);
}

void UCharacterOverlayWidget::SetCharacterStat(UCharacterStatComponent* StatComponent)
{
	StatWidget->SetCharacterStat(StatComponent);
}
