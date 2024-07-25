// Fill out your copyright notice in the Description page of Project Settings.


#include "CastingSkillWidget.h"
#include "Components/ProgressBar.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"

void UCastingSkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsActivated)
	{
		SetCastingBar(InDeltaTime);
	}
}

void UCastingSkillWidget::StartCasting(float MaxValueToSet)
{
	MaxValue = MaxValueToSet;
	CurrentValue = 0;
	bIsActivated = true;
}

void UCastingSkillWidget::SetCastingBar(float ValueToAdd)
{
	CurrentValue += ValueToAdd;
	CastingBar->SetPercent(CurrentValue / MaxValue);

	if (IsCastingEnd())
	{
		bIsActivated = false;

		if (USkillManagerSubSystem* SubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->OnCastingSkillEnd.Broadcast();
		}

		RemoveFromParent();
	}
}

bool UCastingSkillWidget::IsCastingEnd()
{
	return CurrentValue >= MaxValue;
}
