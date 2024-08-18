// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorComponentBase.h"
#include "P1/Widget/Damage/DamageIndicatorWidget.h"
#include "P1/DamageIndicator/DamageIndicatorActor.h"

void UDamageIndicatorComponentBase::ActivateDamageIndicatorWidget(ADamageIndicatorActor* DamageIndicatorActor, float Damage, bool bIsEnemy)
{
	UDamageIndicatorWidget* DamageIndicatorWidget = Cast<UDamageIndicatorWidget>(GetUserWidgetObject());
	DamageIndicatorWidget->ActivateDamageIndicatorWidget(DamageIndicatorActor, Damage, bIsEnemy);
}
