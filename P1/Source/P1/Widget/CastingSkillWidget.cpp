// Fill out your copyright notice in the Description page of Project Settings.


#include "CastingSkillWidget.h"
#include "Components/ProgressBar.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"

void UCastingSkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsActivated)
	{
		SetGaugeBar(InDeltaTime);
	}
}

void UCastingSkillWidget::StartGauge(float MaxValueToSet)
{
	MaxValue = MaxValueToSet;
	CurrentValue = 0;
	bIsActivated = true;
}

void UCastingSkillWidget::SetGaugeBar(float ValueToAdd)
{
	CurrentValue += ValueToAdd;
	CastingBar->SetPercent(CurrentValue / MaxValue);

	if (IsGaugeEnd())
	{
		bIsActivated = false;

		if (USkillManagerSubSystem* SubSystem = GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->OnSkillGaugeEnd.Broadcast();
		}

		RemoveFromParent();
	}
}

bool UCastingSkillWidget::IsGaugeEnd()
{
	return CurrentValue >= MaxValue;
}
