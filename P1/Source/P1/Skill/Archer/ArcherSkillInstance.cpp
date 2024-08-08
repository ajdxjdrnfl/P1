// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherSkillInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "P1/Skill/Manager/CastingSkillManager.h"
#include "P1/Skill/Manager/ChargingSkillManager.h"
#include "P1/Skill/Manager/HoldingSkillManager.h"
#include "P1/Skill/HoldingByTickSkillManager.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"

void AArcherQSkillInstance::UseSkill()
{
	if (CastingSkillManager == nullptr)
	{
		CastingSkillManager = Cast<ACastingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(ACastingSkillManager::StaticClass()));
		CastingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	}

	CastingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
	CastingSkillManager->StartCasting(SkillInfo.CastingTime);
}

void AArcherQSkillInstance::SpawnSkill()
{
	float GaugeRate = 0.f;
	if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		GaugeRate = SubSystem->CastingSkillGaugeRate;
	}

	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(0);

	Pkt.set_damage(GaugeRate * SkillInfo.Damage);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	FVector SpawnLocation = OwnerCreature->GetActorLocation();
	Pkt.set_x(SpawnLocation.X);
	Pkt.set_y(SpawnLocation.Y);
	Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void AArcherQSkillInstance::OnCastingEnd()
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
