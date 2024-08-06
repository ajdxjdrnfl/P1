// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillActorBase.h"
#include "P1/P1.h"
#include "P1/P1Creature.h"
#include "P1/P1Character.h"
#include "P1/Enemy/EnemyBase.h"

ASkillActorBase::ASkillActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ObjectInfo = new Protocol::ObjectInfo();
	//SkillInfo = new Protocol::SkillInfo();
}

void ASkillActorBase::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ASkillActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentLifeTime > 0)
	{
		CurrentLifeTime -= DeltaTime;
	}
	else
	{
		if (this != nullptr)
			Destroy();
	}

}

void ASkillActorBase::InitOnSpawn(class AP1Creature* _OwnerCreature)
{
	OwnerCreature = _OwnerCreature;
	CurrentLifeTime = SkillInfo.LifeTime;
	if (CurrentLifeTime == -1) CurrentLifeTime = 100000;
}

void ASkillActorBase::ActivateSkill()
{

}

void ASkillActorBase::SendCollisionPacketToServer(AP1Creature* Creature)
{
	Protocol::C_ATTACK Pkt;
	Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
	Protocol::ObjectInfo* SkillActorInfoLocal = Pkt.mutable_skillactor();
	Protocol::ObjectInfo* VictimInfo = Pkt.mutable_victim();

	AP1Creature* InstigatorLocal = Cast<AP1Creature>(InstigatorOfSkill);

	if (InstigatorLocal == nullptr)
		return;

	UP1GameInstance* P1GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance());
	if (P1GameInstance == nullptr)
		return;

	if (!P1GameInstance->IsMyCharacter(InstigatorLocal->ObjectInfo->object_id()))
		return;

	if (P1GameInstance->ClassCasterMap.Contains(InstigatorLocal->GetClassType()))
	{
		CasterInfo->set_castertype(P1GameInstance->ClassCasterMap[InstigatorLocal->GetClassType()]);
	}
	if (P1GameInstance->ClassCasterMap.Contains(Creature->GetClassType()))
	{
		VictimInfo->set_castertype(P1GameInstance->ClassCasterMap[Creature->GetClassType()]);
	}

	CasterInfo->CopyFrom(*InstigatorLocal->ObjectInfo);
	VictimInfo->CopyFrom(*Creature->ObjectInfo);
	SkillActorInfoLocal->CopyFrom(*ObjectInfo);

	SEND_PACKET(Pkt);
}

