// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSkillInstance.h"
#include "P1/P1Creature.h"
#include "P1/Skill/SkillActorBase.h"
#include "Kismet/GameplayStatics.h"

void ABossQSkillInstance::SpawnSkill()
{
	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(0);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ASkillActorBase* CurrentSkillActor = Cast<ASkillActorBase>(SkillInfo.SkillActorClass->GetDefaultObject());

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void ABossQSkillInstance::UseSkill()
{
	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	AnimInstance->Montage_Play(M_Skill);
	{
		Protocol::C_MONTAGE Pkt;
		Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
		CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
		Pkt.set_isstop(false);
		Pkt.set_id(0);

		SEND_PACKET(Pkt);
	}
}

void ABossWSkillInstance::SpawnSkill()
{
	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(1);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ASkillActorBase* CurrentSkillActor = Cast<ASkillActorBase>(SkillInfo.SkillActorClass->GetDefaultObject());

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void ABossWSkillInstance::UseSkill()
{
	
}
