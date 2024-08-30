// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1.h"
#include "P1/Component/StatComponent/EnemyStatComponent.h"
#include "P1/Component/WidgetComponent/EnemyWidgetComponent.h"
#include "P1/Component/SkillComponent/EnemySkillComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "P1/Enemy/AIControllerEnemy.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
//#include "P1/Component/DamageIndicator/EnemyDamageIndicatorComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComponent = CreateDefaultSubobject<UEnemyStatComponent>(TEXT("StatComponent"));

	WidgetComponent = CreateDefaultSubobject<UEnemyWidgetComponent>(TEXT("WidgetComponent"));

	SkillComponent = CreateDefaultSubobject<UEnemySkillComponent>(TEXT("SkillComponent"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBase::Init()
{
	if (StatComponent)
	{
		StatComponent->InitStat();
		StatComponent->OwnerEnemy = this;
	}
	if (WidgetComponent && StatComponent)
	{
		WidgetComponent->SetEnemyStat(StatComponent);
		WidgetComponent->OwnerEnemy = this;
		StatComponent->OwnerEnemy = this;
	}
	if (SkillComponent)
	{
		SkillComponent->SetSkills();
	}
}

void AEnemyBase::InitOnSpawn(Protocol::ObjectInfo& ObjInfo)
{
	if (StatComponent)
	{
		StatComponent->OwnerEnemy = this;
		StatComponent->InitOnSpawn(ObjInfo);
	}
	if (WidgetComponent && StatComponent)
	{
		WidgetComponent->InitOnSpawn(StatComponent);
		WidgetComponent->OwnerEnemy = this;
		StatComponent->OwnerEnemy = this;
	}
	if (SkillComponent)
	{
		SkillComponent->InitOnSpawn();
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (StatComponent)
	{
		StatComponent->OwnerEnemy = this;
	}

	if (WidgetComponent)
	{
		WidgetComponent->OwnerEnemy = this;
	}

	if (SkillComponent)
	{
		SkillComponent->OwnerCreature = this;
		SkillComponent->OwnerEnemy = this;
	}
}

void AEnemyBase::SetHealthByDamage(float HealthToSet, ASkillActorBase* HitSkill)
{
	WidgetComponent->SpawnDamageIndicator(ObjectInfo->hp() - HealthToSet);
	StatComponent->SetHealth(HealthToSet);
	SetHitMaterial();

	SkillComponent->SetCC(HitSkill);
}

void AEnemyBase::SetHealthByDamageByDot(float HealthToSet)
{
	if (WidgetComponent)
	{
		WidgetComponent->SpawnDamageIndicator(ObjectInfo->hp() - HealthToSet);
	}

	if (StatComponent)
	{
		StatComponent->SetHealth(HealthToSet);
		SetHitMaterial();
	}
}

void AEnemyBase::Die()
{
	WidgetComponent->AllStop();
	StatComponent->AllStop();

	Super::Die();
}

FSkillInfo AEnemyBase::GetSkillInfoByIndex(int32 SkillIndex)
{
	return SkillComponent->GetSkillInfoByIndex(SkillIndex);
}

void AEnemyBase::SetSpawnedSkill(int32 SkillID, ASkillActorBase* SkillActor)
{
	if (SkillComponent)
	{
		SkillComponent->SetSpawnedSkill(SkillID, SkillActor);
	}
}

void AEnemyBase::TickMove(float DeltaTime)
{
	FVector TargetLocation = FVector(TargetInfo->x(), TargetInfo->y(), GetActorLocation().Z);
	AAIController* AIEnemyController = Cast<AAIController>(GetController());
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();

	if (AIEnemyController == nullptr) 
		return;

	EPathFollowingRequestResult::Type Result = AIEnemyController->MoveToLocation(TargetLocation, CapsuleRadius);

	switch (Result)
	{
	case EPathFollowingRequestResult::AlreadyAtGoal:
	{
		FRotator TargetRotation = FRotator(GetActorRotation().Pitch, TargetInfo->yaw(), GetActorRotation().Roll);
		FRotator NextRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10);
		SetActorRotation(NextRotation);
	}
	break;
	default:
		break;
	}
	

	SetObjectInfo();
}

void AEnemyBase::TickStun()
{
	if (bDoOnceStun) return;
	bDoOnceStun = true;

	if (SkillComponent == nullptr) return;
	SkillComponent->ProcStun();
}

void AEnemyBase::PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt)
{
	if (SkillComponent == nullptr) return;
	SkillComponent->PlayAnimMontageByServer(pkt);
}

