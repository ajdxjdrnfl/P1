// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorComponentBase.h"
#include "P1/Widget/Damage/DamageIndicatorWidget.h"
#include "P1/DamageIndicator/DamageIndicatorActor.h"
#include "P1/Widget/Damage/DamageIndicatorWidget.h"

void UDamageIndicatorComponentBase::ActivateDamageIndicatorWidget(ADamageIndicatorActor* DamageIndicatorActor, float Damage, bool bIsEnemy)
{
	UDamageIndicatorWidget* DamageIndicatorWidget = Cast<UDamageIndicatorWidget>(GetUserWidgetObject());
	if (Damage > 0)
	{
		DamageIndicatorWidget->SetDamageIndicatorTextColor(FLinearColor::Red);
	}
	else
	{
		Damage *= -1;
		DamageIndicatorWidget->SetDamageIndicatorTextColor(FLinearColor::White);
	}
	DamageIndicatorWidget->ActivateDamageIndicatorWidget(DamageIndicatorActor, Damage, bIsEnemy);
}
