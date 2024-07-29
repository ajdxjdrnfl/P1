// Fill out your copyright notice in the Description page of Project Settings.


#include "CastingSkillManager.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/Component/WidgetComponent/CharacterWidgetComponent.h"
#include "P1/Widget/CharacterOverlayWidget.h"

void UCastingSkillManager::Init(class AP1Character* OwnerChar, FSkillInfo SkillInfo)
{
	OwnerCharacter = OwnerChar;
	CurrentSkillInfo = SkillInfo;
}

void UCastingSkillManager::UseSkill()
{
	if (OwnerCharacter->GetSkillComponent()->GetSkillState() != ESkillState::Normal && OwnerCharacter->GetSkillComponent()->GetSkillState() != ESkillState::Casting)
		return;

	// TODO: Do not use Enemy
	if (OwnerCharacter->GetSkillComponent()->GetSkillState() == ESkillState::Casting)
	{
		OwnerCharacter->GetSkillComponent()->SetSkillState(ESkillState::Normal);
		OwnerCharacter->CloseSkillGaugeWidget();
		
		if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
			SubSystem->OnSkillGaugeEnd.Clear();

		return;
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->OnSkillGaugeEnd.AddUniqueDynamic(this, &UCastingSkillManager::OnCastingEnd);
	}

	OwnerCharacter->GetSkillComponent()->SetSkillState(ESkillState::Casting);
	OwnerCharacter->OpenSkillGaugeWidget();

}

void UCastingSkillManager::OnCastingEnd()
{
	OwnerCharacter->GetSkillComponent()->SetSkillState(ESkillState::Normal);
	OwnerCharacter->CloseSkillGaugeWidget();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	FVector Loc = OwnerCharacter->GetActorLocation();
	FRotator Rot = OwnerCharacter->GetActorRotation();

	// TODO: SendPacket
	ASkillActorBase* SpawnedSkillActor = Cast<ASkillActorBase>(OwnerCharacter->GetWorld()->SpawnActor(CurrentSkillInfo.SkillActorClass, &Loc, &Rot, SpawnParams));
	if (SpawnedSkillActor == nullptr)
		return;

	SpawnedSkillActor->ActivateSkill();

	OwnerCharacter->GetWidgetComponent()->GetCharacterOverlayWidget()->UseSkill(CurrentSkillInfo);

	if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		SubSystem->OnSkillGaugeEnd.Clear();

}
