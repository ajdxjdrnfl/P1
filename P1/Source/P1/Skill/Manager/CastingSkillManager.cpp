// Fill out your copyright notice in the Description page of Project Settings.


#include "CastingSkillManager.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/P1Character.h"
#include "P1/P1PlayerController.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/Component/WidgetComponent/CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"
#include "P1/Skill/SkillInstanceBase.h"

void ACastingSkillManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// TODO:
	if (AP1PlayerController* PlayerController = Cast<AP1PlayerController>(OwnerCharacter->GetController()))
	{
		/*if (PlayerController->IsInputKeyDown())
		{

		}*/
	}
}

void ACastingSkillManager::StartCasting(float CastingTime)
{

	if (GetSkillState() != ESkillType::Normal && GetSkillState() != ESkillType::Casting)
		return;

	// TODO: Do not use Enemy
	if (GetSkillState() == ESkillType::Casting)
	{
		SetSkillState(ESkillType::Normal);
		OwnerCharacter->CloseSkillGaugeWidget();

		if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->OnSkillGaugeEnd.Clear();
			SubSystem->SetKeyCanUse(-1);
		}

		return;
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.AddUniqueDynamic(this, &ACastingSkillManager::OnCastingEnd);
		SubSystem->SetKeyCanUse(SkillInfo.SkillNumLocal);
	}

	SetSkillState(ESkillType::Casting);
	OwnerCharacter->OpenSkillGaugeWidget(CastingTime);

}

void ACastingSkillManager::OnCastingEnd()
{
	SetSkillState(ESkillType::Normal);
	OwnerCharacter->CloseSkillGaugeWidget();
	OwnerCharacter->GetWidgetComponent()->GetCharacterOverlayWidget()->UseSkill(SkillInfo);

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.Clear();
		SubSystem->SetKeyCanUse(-1);
	}
		
	if (SkillInstance)
	{
		SkillInstance->SpawnSkill();
	}
}
