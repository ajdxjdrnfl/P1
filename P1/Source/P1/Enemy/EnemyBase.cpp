// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1.h"
#include "P1/Component/StatComponent/EnemyStatComponent.h"
#include "P1/Component/WidgetComponent/EnemyWidgetComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StatComponent = CreateDefaultSubobject<UEnemyStatComponent>(TEXT("StatComponent"));

	WidgetComponent = CreateDefaultSubobject<UEnemyWidgetComponent>(TEXT("WidgetComponent"));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnCollisionOverlapBegin);

	// TODO: to on spawn
	//Init();
}

void AEnemyBase::Init()
{
	if (StatComponent)
	{
		StatComponent->InitStat();
	}
	if (WidgetComponent && StatComponent)
	{
		WidgetComponent->SetEnemyStat(StatComponent);
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

void AEnemyBase::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASkillActorBase* SkillActor = Cast<ASkillActorBase>(OtherActor))
	{
		
	}
}

