// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillInstanceBase.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1GameInstance.h"

ASkillInstanceBase::ASkillInstanceBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
}

void ASkillInstanceBase::Init(AP1Creature* _OwnerCreature)
{
	OwnerCreature = _OwnerCreature;

	check(OwnerCreature != nullptr);
}

void ASkillInstanceBase::DespawnSkill()
{
	UP1GameInstance* GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance == nullptr) return;

	ASkillActorBase* CurrentSkillActor = Cast<ASkillActorBase>(SkillActorClass->GetDefaultObject());
	if (CurrentSkillActor == nullptr) return;
	
	GameInstance->DespawnSkill(CurrentSkillActor->SkillInfo->skill_id());
}

void ASkillInstanceBase::StartMontage(int32 SkillIndexLocal)
{
	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(nullptr);
	if (!bIsMontagePlaying)
	{
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(false);
			Pkt.set_id(SkillIndexLocal);

			SEND_PACKET(Pkt);
		}
	}
}

void ASkillInstanceBase::StopMontage(int32 SkillIndexLocal)
{
	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(nullptr);
	if (bIsMontagePlaying)
	{
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(true);
			Pkt.set_id(SkillIndexLocal);

			SEND_PACKET(Pkt);
		}
	}
}
