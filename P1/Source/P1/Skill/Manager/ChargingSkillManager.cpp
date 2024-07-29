// Fill out your copyright notice in the Description page of Project Settings.


#include "ChargingSkillManager.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/Component/WidgetComponent/CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"
#include "P1/Skill/SkillInstanceBase.h"

void AChargingSkillManager::Init(AP1Character* _OwnerCharacter, ASkillInstanceBase* _SkillInstance, FSkillInfo _SkillInfo)
{
	OwnerCharacter = _OwnerCharacter;
	SkillInstance = _SkillInstance;
	SkillInfo = _SkillInfo;
}

void AChargingSkillManager::StartCasting()
{
	if (GetSkillState() != ESkillState::Normal && GetSkillState() != ESkillState::Charging)
		return;

	// TODO: Do not use Enemy
	if (GetSkillState() == ESkillState::Charging)
	{
		FireSkill(OwnerCharacter->GetGaugeRate());
		return;
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.AddUniqueDynamic(this, &AChargingSkillManager::OnCastingEnd);
	}

	SetSkillState(ESkillState::Charging);
	OwnerCharacter->OpenSkillGaugeWidget();
}

void AChargingSkillManager::FireSkill(float rate = 1.f)
{
	SetSkillState(ESkillState::Normal);
	OwnerCharacter->CloseSkillGaugeWidget();
	OwnerCharacter->GetWidgetComponent()->GetCharacterOverlayWidget()->UseSkill(SkillInfo);

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.Clear();
		SubSystem->CastingSkillGaugeRate = rate;
	}

	if (SkillInstance)
	{
		SkillInstance->SpawnSkill();
	}

}

void AChargingSkillManager::OnCastingEnd()
{
	FireSkill();
}
