// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillInstanceBase.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1GameInstance.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"

ASkillInstanceBase::ASkillInstanceBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
}

void ASkillInstanceBase::Init(AP1Creature* _OwnerCreature)
{
	OwnerCreature = _OwnerCreature;

	check(OwnerCreature != nullptr);


	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr)
		return;

	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &ASkillInstanceBase::OnMontageEnded);
}

void ASkillInstanceBase::ActivateSkill(ASkillActorBase* SkillActor)
{
	if (SkillActor == nullptr) return;

	SkillActor->ActivateSkill();
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

void ASkillInstanceBase::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (AnimMontage == M_Skill)
	{
		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->SetKeyCanUse(-1);
		}
	}
}

