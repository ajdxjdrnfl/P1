// Fill out your copyright notice in the Description page of Project Settings.


#include "MobSkillInstance.h"
#include "P1/P1.h"
#include "P1/P1Creature.h"
#include "P1/Skill/SkillActorBase.h"

void AMobQSkillInstance::UseSkill()
{
	{
		Protocol::C_MONTAGE Pkt;
		Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
		CasterInfo->CopyFrom(*OwnerCreature->ObjectInfo);
		Pkt.set_isstop(false);
		Pkt.set_id(0);

		SEND_PACKET(Pkt);
	}
}

void AMobQSkillInstance::SpawnSkill()
{
	{
		if (SkillActorClass == nullptr) return;

		Protocol::C_SKILL Pkt;
		Pkt.set_skillid(0);
		Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

		ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

		FVector SpawnLocation = OwnerCreature->GetActorLocation() + OwnerCreature->GetActorForwardVector() * 200.f;
		Pkt.set_x(SpawnLocation.X);
		Pkt.set_y(SpawnLocation.Y);
		Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

		SEND_PACKET(Pkt);
	}
}
