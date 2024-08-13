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

void AHoldingSkillManager::BeginPlay()
{
	Super::BeginPlay();
}

void AHoldingSkillManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActivated) return;
	if (bIsTick) return;

	if (AttackTime >= CurrentTime)
	{
		if (SkillInstance)
		{
			SkillInstance->SpawnSkill();
		}
		CurrentTime = 0.f;
	}
	else
	{
		CurrentTime += DeltaTime;
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
		SubSystem->SetKeyCanUse(SkillInfo.SkillNumLocal);
	}

	SetSkillState(ESkillType::Hold);
	OwnerCharacter->OpenSkillGaugeWidget(CastingTime);
	SkillInstance->StartMontage(SkillInfo.SkillNumLocal);
	bIsActivated = true;
	
}

void AHoldingSkillManager::EndSkill()
{
	SetSkillState(ESkillType::Normal);
	OwnerCharacter->CloseSkillGaugeWidget();
	OwnerCharacter->GetWidgetComponent()->GetCharacterOverlayWidget()->UseSkill(SkillInfo);

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.Clear();
		SubSystem->SetKeyCanUse(-1);
	}

	SkillInstance->StopMontage(SkillInfo.SkillNumLocal);
	bIsActivated = false;

	Destroy();
}

void AHoldingSkillManager::OnCastingEnd()
{
	EndSkill();
}