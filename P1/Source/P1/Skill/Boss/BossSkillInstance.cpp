// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSkillInstance.h"
#include "P1/P1Creature.h"
#include "P1/Skill/SkillActorBase.h"
#include "Kismet/GameplayStatics.h"
#include "P1/Enemy/EnemyBoss.h"

void ABossQSkillInstance::SpawnSkill()
{
	/*if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(0);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);*/
}

void ABossQSkillInstance::UseSkill()
{
	/*UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	AnimInstance->Montage_Play(M_Skill);
	{
		Protocol::C_MONTAGE Pkt;
		Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
		CasterInfo->CopyFrom(*OwnerCreature->ObjectInfo);
		Pkt.set_isstop(false);
		Pkt.set_id(0);

		SEND_PACKET(Pkt);
	}*/
}

void ABossWSkillInstance::SpawnSkill()
{
	/*if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(1);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);*/
}

void ABossWSkillInstance::SpawnSkillAtLocation(FVector2D Location)
{
	if (SkillActorClass == nullptr) return;

	/*Protocol::C_SKILL Pkt;
	Pkt.set_skillid(1);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);
	ObjectInfoRef->set_x(Location.X);
	ObjectInfoRef->set_y(Location.Y);

	SEND_PACKET(Pkt);*/

	OwnerCreature->SetWalkSpeed(OwnerCreature->DefaultWalkSpeed);
}

void ABossWSkillInstance::UseSkill()
{
	//PlayVanishMontage();
}

void ABossWSkillInstance::ActivateSkill(ASkillActorBase* SkillActor)
{
	SkillActor->SpawnActivationParticleOnLocation(SkillActor->GetActorLocation());
}

void ABossWSkillInstance::MoveToMiddle()
{
	{
		Protocol::C_MOVE Pkt;
		Protocol::ObjectInfo* info = Pkt.mutable_info();
		// TODO: State

		OwnerCreature->ObjectInfo->set_x(0);
		OwnerCreature->ObjectInfo->set_y(0);
		OwnerCreature->ObjectInfo->set_z(0);
		OwnerCreature->ObjectInfo->set_yaw(0);

		info->CopyFrom(*OwnerCreature->ObjectInfo);

		SEND_PACKET(Pkt);
	}
}

void ABossWSkillInstance::SpawnPillar()
{
	if (SkillActorClass == nullptr) return;

	for (int i = 0; i < 4; i++)
	{
		Protocol::C_SKILL Pkt;
		Pkt.set_skillid(1);
		Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

		ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);
		ObjectInfoRef->set_castertype(Protocol::CASTER_TYPE_STRUCTURE);

		SEND_PACKET(Pkt);
	}
}

void ABossWSkillInstance::RushToPillar()
{
	//OwnerCreature->SetWalkSpeed(800);
	// Cast<AEnemyBoss>(OwnerCreature)->SetAttackMode(true);
}

void ABossESkillInstance::ActivateSkill(ASkillActorBase* SkillActor)
{
	SkillActor->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
}

void ABossESkillInstance::OnSkillDetroyed()
{
	OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Stop(1.f, M_Skill);
}
