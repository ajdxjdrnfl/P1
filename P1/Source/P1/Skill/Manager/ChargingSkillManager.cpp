// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargingSkillManager.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/Component/WidgetComponent/CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"
#include "P1/Skill/SkillInstanceBase.h"

void AChargingSkillManager::StartCasting(float CastingTime)
{

	if (GetSkillState() != ESkillType::Normal && GetSkillState() != ESkillType::Charging)
		return;

	// TODO: Do not use Enemy
	if (GetSkillState() == ESkillType::Charging)
	{
		FireSkill(OwnerCharacter->GetGaugeRate());
		return;
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.AddUniqueDynamic(this, &AChargingSkillManager::OnCastingEnd);
		SubSystem->SetKeyCanUse(SkillInfo.SkillNumLocal);
	}

	SetSkillState(ESkillType::Charging);
	OwnerCharacter->OpenSkillGaugeWidget(CastingTime);
}

void AChargingSkillManager::FireSkill(float rate = 1.f)
{
	SetSkillState(ESkillType::Normal);
	OwnerCharacter->CloseSkillGaugeWidget();
	OwnerCharacter->GetWidgetComponent()->GetCharacterOverlayWidget()->UseSkill(SkillInfo);

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.Clear();
		SubSystem->CastingSkillGaugeRate = rate;
		SubSystem->SetKeyCanUse(-1);
	}

	if (SkillInstance)
	{
		SkillInstance->OnCastingEnd();
	}
}

void AChargingSkillManager::OnCastingEnd()
{
	FireSkill();
	Destroy();
}
