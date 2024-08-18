// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWidgetComponent.h"
#include "P1/Enemy/EnemyBase.h"
#include "P1/Component/StatComponent/EnemyStatComponent.h"
#include "P1/Widget/Stat/EnemyStatWidget.h"
#include "Components/WidgetComponent.h"
#include "P1/DamageIndicator/DamageIndicatorActor.h"

void UEnemyWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UEnemyWidgetComponent::InitOnSpawn(UStatComponentBase* StatComponent)
{
	Super::InitOnSpawn(StatComponent);
	Cast<AEnemyBase>(GetOwner())->GetStatComponent()->OnEnemyHealthChangedDelegate.AddDynamic(this, &UEnemyWidgetComponent::OnEnemyHealthChanged);
	SetEnemyStat(Cast<UEnemyStatComponent>(StatComponent));
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
	if (StatWidget == nullptr)
		return;

	StatWidget->RemoveFromParent();
	StatWidget = nullptr;
}

void UEnemyWidgetComponent::SetStatWidget(UEnemyStatWidget* StatWidgetToSet)
{
	StatWidget = StatWidgetToSet;
}

void UEnemyWidgetComponent::SpawnDamageIndicator(float Damage)
{
	FVector SpawnLocation = OwnerEnemy->GetActorLocation();
	ADamageIndicatorActor* DamageIndicator = Cast<ADamageIndicatorActor>(GetWorld()->SpawnActor(DamageIndicatorActorClass, &SpawnLocation));
	if (DamageIndicator)
	{
		DamageIndicator->ActivateDamageIndicator(Damage, true);
	}
}
