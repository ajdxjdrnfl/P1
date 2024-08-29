// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBoss.h"
#include "P1/Enemy/AIControllerEnemy.h"
#include "P1/Component/SkillComponent/EnemySkillComponent.h"
#include "P1/Widget/Stat/BossStatWidget.h"
#include "P1/Component/WidgetComponent/EnemyWidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "P1/P1Character.h"
#include "P1/P1GameInstance.h"

void AEnemyBoss::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AAIControllerEnemy>(GetController());

	if (HealthBarWidgetClass)
	{
		UBossStatWidget* StatWidget = Cast<UBossStatWidget>(CreateWidget(GetWorld(), HealthBarWidgetClass));
		StatWidget->AddToViewport();
		GetWidgetComponent()->SetStatWidget(StatWidget);
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBoss::OnCollisionOverlapBegin);

	BossSkillState.SetNum(4);
}

void AEnemyBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (ObjectInfo->state())
	{
	case Protocol::MOVE_STATE_IDLE:
		break;
	case Protocol::MOVE_STATE_SKILL:
	case Protocol::MOVE_STATE_RUN:
		TickMove(DeltaTime);
		break;
	case Protocol::MOVE_STATE_STUN:
		TickStun();
		break;
	default:
		break;
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

	EnemyController->MoveToTargetActor(TargetActor);
}

void AEnemyBoss::UseSkill(int32 SkillIndex)
{
	if (SkillComponent == nullptr) return;

	SkillComponent->UseSkill(SkillIndex);
}

void AEnemyBoss::UsePillarSkill(FVector2D Location)
{
	if (SkillComponent == nullptr) return;

	SkillComponent->UsePillarSkill(Location);
}

void AEnemyBoss::SetAttackMode(bool bAttackMode)
{
	bIsAttackMode = bAttackMode;
}

void AEnemyBoss::PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt)
{
	BossSkillStateIndex = pkt.section_num();
	BossSkillID = pkt.id();

	if (pkt.section_num() < 1)
	{
		Super::PlayAnimMontageByServer(pkt);
		if (pkt.isstop())
		{
			BossSkillState[pkt.id()] = 0;
			OnSkillEndDelegate.Broadcast(pkt.id());
		}
		return;
	}

	BossSkillState[pkt.id()] = pkt.section_num();
}

void AEnemyBoss::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsAttackMode) return;
}
