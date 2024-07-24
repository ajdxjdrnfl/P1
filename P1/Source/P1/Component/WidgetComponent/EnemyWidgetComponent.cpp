// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidgetComponent.h"
#include "P1/Enemy/EnemyBase.h"
#include "P1/Component/StatComponent/EnemyStatComponent.h"
#include "P1/Widget/Stat/EnemyStatWidget.h"
#include "Components/WidgetComponent.h"

void UEnemyWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	Cast<AEnemyBase>(GetOwner())->GetStatComponent()->OnEnemyHealthChangedDelegate.AddDynamic(this, &UEnemyWidgetComponent::OnEnemyHealthChanged);
}

void UEnemyWidgetComponent::OnEnemyHealthChanged()
{
	if (StatWidget == nullptr)
		return;

	StatWidget->SetHealthBar();
}

void UEnemyWidgetComponent::SetEnemyStat(UEnemyStatComponent* StatComponent)
{
	if (StatWidget == nullptr)
		return;

	StatWidget->SetEnemyStat(StatComponent);
}
