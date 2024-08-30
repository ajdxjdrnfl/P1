// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkillComponent.h"
#include "P1/Skill/Boss/BossSkillInstance.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/Enemy/EnemyBase.h"

void UEnemySkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEnemySkillComponent::InitOnSpawn()
{
	Super::InitOnSpawn();

	OwnerEnemy->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(this, &UEnemySkillComponent::OnMontageEnded);
}

void UEnemySkillComponent::UseSkill(int32 SkillIndex)
{
	Super::UseSkill(SkillIndex);
}

void UEnemySkillComponent::UsePillarSkill(FVector2D Location)
{
	if (GetSkillInstances().Num() <= 1 || GetSkillInstances()[1] == nullptr) return;

	GetSkillInstances()[1]->SpawnSkillAtLocation(Location);
}

void UEnemySkillComponent::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte)
{
	if (GetStunMontage())
	{
		if (AnimMontage == GetStunMontage())
		{
			OwnerEnemy->ObjectInfo->set_state(Protocol::MOVE_STATE_IDLE);
			OwnerEnemy->SetDoOnceStun(false);
		}
	}
}
