// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "P1/Enemy/AIControllerEnemy.h"
#include "P1/Component/SkillComponent/EnemySkillComponent.h"
#include "P1/Widget/Stat/BossStatWidget.h"
#include "P1/Component/WidgetComponent/EnemyWidgetComponent.h"

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AAIControllerEnemy>(AIControllerClass);

	if (HealthBarWidgetClass)
	{
		UBossStatWidget* StatWidget = Cast<UBossStatWidget>(CreateWidget(GetWorld(), HealthBarWidgetClass));
		StatWidget->AddToViewport();
		GetWidgetComponent()->SetStatWidget(StatWidget);
	}
}

void AEnemyBoss::MoveToTargetLocation(FVector TargetLoc)
{
	if (EnemyController == nullptr) return;

	EnemyController->MoveToTargetLocation(TargetLoc);
}

void AEnemyBoss::MoveToTargetActor(AActor* TargetActor)
{
	if (EnemyController == nullptr) return;

	EnemyController->MoveToTargetActor(TargetActor );
}

void AEnemyBoss::UseSkill(int32 SkillIndex)
{
	if (SkillComponent == nullptr) return;

	SkillComponent->UseSkill(SkillIndex);
}
