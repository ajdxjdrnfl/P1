// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSkillInstance.h"
#include "P1/P1Creature.h"
#include "P1/Skill/SkillActorBase.h"

void ABossQSkillInstance::SpawnSkill()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCreature;
	SpawnParams.Instigator = OwnerCreature;

	FVector Loc = OwnerCreature->GetActorLocation() + OwnerCreature->GetActorForwardVector() * 200.f;
	FRotator Rot = OwnerCreature->GetActorRotation();

	if (SkillActorClass == nullptr) return;

	// TODO: SendPacket
	SpawnedSkillActor = Cast<ASkillActorBase>(OwnerCreature->GetWorld()->SpawnActor(SkillActorClass, &Loc, &Rot, SpawnParams));
	if (SpawnedSkillActor == nullptr) return;

	SpawnedSkillActor->InitOnSpawn(OwnerCreature);
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
