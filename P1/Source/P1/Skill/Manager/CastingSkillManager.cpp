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

void ACastingSkillManager::Init(class AP1Character* _OwnerCharacter, class ASkillInstanceBase* _SkillInstance, FSkillInfo _SkillInfo)
{
	OwnerCharacter = _OwnerCharacter;
	SkillInstance = _SkillInstance;
	SkillInfo = _SkillInfo;
}

void ACastingSkillManager::StartCasting()
{
	if (GetSkillState() != ESkillState::Normal && GetSkillState() != ESkillState::Casting)
		return;

	// TODO: Do not use Enemy
	if (GetSkillState() == ESkillState::Casting)
	{
		SetSkillState(ESkillState::Normal);
		OwnerCharacter->CloseSkillGaugeWidget();

		if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
			SubSystem->OnSkillGaugeEnd.Clear();

		return;
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.AddUniqueDynamic(this, &ACastingSkillManager::OnCastingEnd);
	}

	SetSkillState(ESkillState::Casting);
	OwnerCharacter->OpenSkillGaugeWidget();

}

void ACastingSkillManager::OnCastingEnd()
{
	SetSkillState(ESkillState::Normal);
	OwnerCharacter->CloseSkillGaugeWidget();
	OwnerCharacter->GetWidgetComponent()->GetCharacterOverlayWidget()->UseSkill(SkillInfo);

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		SubSystem->OnSkillGaugeEnd.Clear();

	if (SkillInstance)
	{
		SkillInstance->SpawnSkill();
	}
}
