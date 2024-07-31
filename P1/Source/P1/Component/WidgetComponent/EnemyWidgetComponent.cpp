// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidgetComponent.h"
#include "P1/Enemy/EnemyBase.h"
#include "P1/Component/StatComponent/EnemyStatComponent.h"
#include "P1/Widget/Stat/EnemyStatWidget.h"
#include "Components/WidgetComponent.h"

void UEnemyWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO: later move to OnSpawn function
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

void UEnemyWidgetComponent::AllStop()
{
	StatWidget->RemoveFromParent();
	StatWidget = nullptr;
}

void UEnemyWidgetComponent::SetStatWidget(UEnemyStatWidget* StatWidgetToSet)
{
	StatWidget = StatWidgetToSet;
}
