// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorSkills.h"
#include "Components/BoxComponent.h"
#include "P1/P1Character.h"
#include "P1/P1PlayerController.h"
#include "P1/Enemy/EnemyBase.h"
#include "Kismet/KismetMathLibrary.h"

AWarriorQSkill::AWarriorQSkill()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AWarriorQSkill::OnCollisionOverlapBegin);
}

void AWarriorQSkill::OnCollisionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor);
	if (Enemy == nullptr) return;

	// TODO:
}

AWarriorWSkill::AWarriorWSkill()
{
}

void AWarriorWSkill::ActivateSkill()
{
}

AWarriorESkill::AWarriorESkill()
{
}

void AWarriorESkill::ActivateSkill()
{
}

AWarriorRSkill::AWarriorRSkill()
{
}

void AWarriorRSkill::ActivateSkill()
{
}
