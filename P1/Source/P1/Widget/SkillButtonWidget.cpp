// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillButtonWidget.h"
#include "Components/WrapBox.h"
#include "SkillSlotWidget.h"
#include "P1/Data/SkillData.h"
#include "P1/Skill/SkillActorBase.h"

void USkillButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (SkillButtons->GetChildrenCount() < SkillSlotNum && KeyString.Num() >= SkillSlotNum)
	{
		for (int i = 0; i < SkillSlotNum; i++)
		{
			if (SkillButtons->GetChildrenCount() >= SkillSlotNum) 
				break;

			if (SkillSlotWidgetClass == nullptr)
				break;

			USkillSlotWidget* SkillSlotWidget = Cast<USkillSlotWidget>(CreateWidget(this, SkillSlotWidgetClass));
			if (SkillSlotWidget == nullptr)
				break;

			SkillSlotWidget->SetSlotID(i);
			SkillSlotWidget->SetKeyText(KeyString[i]);
			SkillButtons->AddChildToWrapBox(SkillSlotWidget);
		}
	}
}

void USkillButtonWidget::UseSkill(FGeneralSkillInfo SkillInfo)
{
	// TODO:
	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(SkillInfo.SkillActor->GetDefaultObject());

	if (SkillActor == nullptr)
		return;

	USkillSlotWidget* CurrentSkillSlotWidget = Cast<USkillSlotWidget>(SkillButtons->GetChildAt(SkillInfo.SkillNum));

	if (CurrentSkillSlotWidget == nullptr) 
		return;

	CurrentSkillSlotWidget->SetSkillNum(SkillInfo.SkillNum);
	CurrentSkillSlotWidget->ActivateSlot(SkillActor->GetSkillInfo().CooldownTime);
}
