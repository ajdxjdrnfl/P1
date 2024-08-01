// Fill out your copyright notice in the Description page of Project Settings.


#include "HoldingSkillManager.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/P1Character.h"
#include "P1/P1PlayerController.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/Component/WidgetComponent/CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"
#include "P1/Skill/SkillInstanceBase.h"

void AHoldingSkillManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttackTime >= CurrentTime)
	{
		if (SkillInstance)
		{
			SkillInstance->SpawnSkill();
		}
		AttackTime = 0.f;
	}
	else
	{
		AttackTime += DeltaTime;
	}
}

void AHoldingSkillManager::StartCasting(float CastingTime)
{

	if (GetSkillState() != ESkillType::Normal && GetSkillState() != ESkillType::Hold)
		return;

	// TODO: Do not use Enemy
	if (GetSkillState() == ESkillType::Hold)
	{
		EndSkill();
		return;
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.AddUniqueDynamic(this, &AHoldingSkillManager::OnCastingEnd);
		SubSystem->CurrentSkillManager = this;
	}

	SetSkillState(ESkillType::Hold);
	OwnerCharacter->OpenSkillGaugeWidget(CastingTime);
	SkillInstance->StartMontage(SkillInfo.SkillNumLocal);
	OwnerCharacter->CreatureState = ECreatureState::Skill;
}

void AHoldingSkillManager::EndSkill()
{
	SetSkillState(ESkillType::Normal);
	OwnerCharacter->CloseSkillGaugeWidget();
	OwnerCharacter->GetWidgetComponent()->GetCharacterOverlayWidget()->UseSkill(SkillInfo);

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.Clear();
		SubSystem->CurrentSkillManager = nullptr;
	}

	SkillInstance->StopMontage(SkillInfo.SkillNumLocal);
	OwnerCharacter->CreatureState = ECreatureState::Idle;
}

void AHoldingSkillManager::OnCastingEnd()
{
	EndSkill();
}
