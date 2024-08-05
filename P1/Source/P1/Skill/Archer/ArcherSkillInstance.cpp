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
	if (ChargingSkillManager == nullptr)
	{
		ChargingSkillManager = Cast<AChargingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(AChargingSkillManager::StaticClass()));
		ChargingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	}

	ChargingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
	ChargingSkillManager->StartCasting(SkillInfo.CastingTime);
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
	Pkt.set_skillid(2);

	Pkt.set_damage(GaugeRate * SkillInfo.Damage);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	FVector SpawnLocation = OwnerCreature->GetActorLocation();
	Pkt.set_x(SpawnLocation.X);
	Pkt.set_y(SpawnLocation.Y);
	Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}
