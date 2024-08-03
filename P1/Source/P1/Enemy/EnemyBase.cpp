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

	Init();
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

void AEnemyBase::InitOnSpawn(float HealthToSet)
{
	if (StatComponent)
	{
		StatComponent->InitStat(HealthToSet);
	}
	if (WidgetComponent && StatComponent)
	{
		WidgetComponent->SetEnemyStat(StatComponent);
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveByServer(DeltaTime);
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
}

void AEnemyBase::OnSpawn(float HealthToSet)
{
	InitOnSpawn(HealthToSet);
}

void AEnemyBase::TakeDamage()
{
	if (StatComponent)
	{
		StatComponent->TakeDamage();
	}
}

void AEnemyBase::SetHealthByDamage(float HealthToSet)
{
	if (StatComponent)
	{
		StatComponent->SetHealth(HealthToSet);
	}
}

void AEnemyBase::Die()
{
	WidgetComponent->AllStop();
	StatComponent->AllStop();

	GetMesh()->GetAnimInstance()->Montage_Play(M_Die);

	Super::Die();
}

FSkillInfo AEnemyBase::GetSkillInfoByIndex(int32 SkillIndex)
{
	return SkillComponent->GetSkillInfoByIndex(SkillIndex);
}

void AEnemyBase::MoveByServer(float DeltaTime)
{
	if (CreatureState == ECreatureState::Move)
	{
		FVector TargetLocation = FVector(TargetInfo->x(), TargetInfo->y(), GetActorLocation().Z);
		FRotator TargetRotation = FRotator(GetActorRotation().Pitch, TargetInfo->yaw(), GetActorRotation().Roll);
		FRotator NextRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.f);

		AAIController* AIEnemyController = Cast<AAIController>(GetController());

		if (AIEnemyController == nullptr) return;

		AIEnemyController->MoveToLocation(TargetLocation);
		SetObjectInfo();
		SetActorRotation(NextRotation);
	}
}

void AEnemyBase::PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt)
{
	if (SkillComponent == nullptr) return;
	SkillComponent->PlayAnimMontageByServer(pkt);
}

