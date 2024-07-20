// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillButtonWidget.h"
#include "Components/WrapBox.h"
#include "SkillSlotWidget.h"

void USkillButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (SkillButtons->GetChildrenCount() < SkillSlotNum)
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
			SkillButtons->AddChildToWrapBox(SkillSlotWidget);
		}
	}
}
