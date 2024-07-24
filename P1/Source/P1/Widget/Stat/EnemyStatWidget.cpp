// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyStatWidget.h"
#include "P1/Component/StatComponent/EnemyStatComponent.h"
#include "Components/ProgressBar.h"

void UEnemyStatWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UEnemyStatWidget::SetHealthBar()
{
	if (EnemyStatComponent == nullptr)
		return;

	float HealthRate = EnemyStatComponent->GetCurrentHealth() / EnemyStatComponent->GetMaxHealth();
	HealthBar->SetPercent(HealthRate);
}

void UEnemyStatWidget::SetEnemyStat(UEnemyStatComponent* StatComponent)
{
	EnemyStatComponent = StatComponent;
	EnemyStatComponent->SetCurrentHealth(EnemyStatComponent->GetMaxHealth());
	SetHealthBar();
}
