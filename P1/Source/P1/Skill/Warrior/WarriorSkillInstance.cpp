// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorSkillInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "P1/Skill/Manager/CastingSkillManager.h"
#include "P1/Skill/Manager/ChargingSkillManager.h"
#include "P1/Skill/Manager/HoldingSkillManager.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"

AWarriorQSkillInstance::AWarriorQSkillInstance()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWarriorQSkillInstance::BeginPlay()
{
	Super::BeginPlay();
}

void AWarriorQSkillInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTurning && OwnerCreature && Cast<UP1GameInstance>(GetWorld()->GetGameInstance())->IsMyCharacter(OwnerCreature->ObjectInfo->object_id()))
	{
		FVector MouseLoc = Cast<AP1PlayerController>(OwnerCreature->GetController())->GetMouseLocation();
		FVector CharacterLoc = FVector(OwnerCreature->GetActorLocation().X, OwnerCreature->GetActorLocation().Y, MouseLoc.Z);
		FVector CharacterToMouseVector = MouseLoc - CharacterLoc;
		CharacterToMouseVector.Normalize();

		FRotator TargetRot = FRotator(CharacterToMouseVector.Rotation());
		OwnerCreature->SetActorRotation(UKismetMathLibrary::RInterpTo(OwnerCreature->GetActorRotation(), TargetRot, DeltaTime, 100));
	}
}

/*
* Call On Anim Notify
*/
void AWarriorQSkillInstance::SpawnSkill()
{
	if (bDoOnceActivated) return;
	bDoOnceActivated = true; // set false at skill end

	Protocol::C_SKILL Pkt;
	Protocol::SkillInfo* SkillInfoRef = Pkt.mutable_skillinfo();
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ASkillActorBase* CurrentSkillActor = Cast<ASkillActorBase>(SkillActorClass->GetDefaultObject());

	SkillInfoRef->CopyFrom(*CurrentSkillActor->SkillInfo);
	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void AWarriorQSkillInstance::UseSkill()
{
	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(nullptr);
	if (!bIsMontagePlaying)
	{
		AttackIndex = 1;
		//AnimInstance->Montage_Play(M_Skill);

		// Send Packet
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(false);
			Pkt.set_id(0);

			SEND_PACKET(Pkt);
		}

		OwnerCreature->CreatureState = ECreatureState::Skill;
	}
	else if (bIsComboTiming)
	{
		AttackIndex++;
		// AnimInstance->Montage_JumpToSection(AttackSectionIndex);
		bIsComboTiming = false;

		// Send Packet
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(false);
			Pkt.set_id(0);
			
			Pkt.set_section_num(FCString::Atoi(*AttackSectionIndex.ToString()));

			SEND_PACKET(Pkt);

			OwnerCreature->CreatureState = ECreatureState::Idle;
		}
	}
}

void AWarriorWSkillInstance::UseSkill()
{
	if (HoldingSkillManager == nullptr)
	{
		HoldingSkillManager = Cast<AHoldingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(AHoldingSkillManager::StaticClass()));
		HoldingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	}

	HoldingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
	HoldingSkillManager->StartCasting(5);
}

void AWarriorWSkillInstance::SpawnSkill()
{
	// TODO: SendPacket
	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Protocol::SkillInfo* SkillInfoRef = Pkt.mutable_skillinfo();
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ASkillActorBase* CurrentSkillActor = Cast<ASkillActorBase>(SkillInfo.SkillActorClass->GetDefaultObject());

	SkillInfoRef->CopyFrom(*CurrentSkillActor->SkillInfo);
	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void AWarriorESkillInstance::UseSkill()
{
	if (ChargingSkillManager == nullptr)
	{
		ChargingSkillManager = Cast<AChargingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(AChargingSkillManager::StaticClass()));
		ChargingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	}

	ChargingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
	ChargingSkillManager->StartCasting(2);
}

void AWarriorESkillInstance::SpawnSkill()
{
	float GaugeRate = 0.f;
	if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		GaugeRate = SubSystem->CastingSkillGaugeRate;
	}

	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Protocol::SkillInfo* SkillInfoRef = Pkt.mutable_skillinfo();
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	ASkillActorBase* CurrentSkillActor = Cast<ASkillActorBase>(SkillInfo.SkillActorClass->GetDefaultObject());

	SkillInfoRef->CopyFrom(*CurrentSkillActor->SkillInfo);
	SkillInfoRef->set_damage(SkillInfoRef->damage() * GaugeRate);

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void AWarriorRSkillInstance::UseSkill()
{
	if (CastingSkillManager == nullptr)
	{
		CastingSkillManager = Cast<ACastingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(ACastingSkillManager::StaticClass()));
		CastingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	}

	CastingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
	CastingSkillManager->StartCasting(2);

	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(M_Skill);
	if (!bIsMontagePlaying)
	{
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(false);
			Pkt.set_id(3);

			SEND_PACKET(Pkt);
		}
	}
	else
	{
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(true);
			Pkt.set_id(3);

			SEND_PACKET(Pkt);
		}
	}

}

void AWarriorRSkillInstance::SpawnSkill()
{
	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(M_Skill);

	if (bIsMontagePlaying)
	{
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(false);
			Pkt.set_id(0);

			Pkt.set_section_num(2);

			SEND_PACKET(Pkt);
		}
	}
}
