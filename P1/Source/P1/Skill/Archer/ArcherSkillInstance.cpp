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
#include "P1/Skill/Targeting/WorldCursorSkillTargeting.h"

void AArcherQSkillInstance::UseSkill()
{
	Super::UseSkill();
	if (!IsValid(ChargingSkillManager))
	{
		ChargingSkillManager = Cast<AChargingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(AChargingSkillManager::StaticClass()));
		ChargingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
		ChargingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
		ChargingSkillManager->StartCasting(SkillInfo.CastingTime);
	}
	else
	{
		ChargingSkillManager->StartCasting(SkillInfo.CastingTime);
		ChargingSkillManager = nullptr;
	}
}

void AArcherQSkillInstance::SpawnSkill()
{
	{
		float GaugeRate = 0.f;
		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			GaugeRate = SubSystem->CastingSkillGaugeRate;
		}

		{
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
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->bCanMove = false;
	}
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

void AArcherQSkillInstance::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (AnimMontage == M_Skill)
	{
		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->bCanMove = true;
		}
	}
}

void AArcherWSkillInstance::UseSkill()
{
	if (!IsValid(SkillTargeting))
	{
		if (IsValid(HoldingByTickSkillManager))
		{
			UseSkillAfterTargetingWithPos(FVector(0));
			return;
		}

		SkillTargeting = Cast<AWorldCursorSkillTargeting>(OwnerCreature->GetWorld()->SpawnActor(SkillInfo.SkillTargeting));
		SkillTargeting->Init(this);

		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->CurrentSkillTargeting = SkillTargeting;
		}
	}
	else
	{
		SkillTargeting->Destroy();
		SkillTargeting = nullptr;
	}
}

void AArcherWSkillInstance::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if ((AnimMontage == M_Skill) && IsValid(CurrentSkillActor))
	{
		CurrentSkillActor->Destroy();
		CurrentSkillActor = nullptr;
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->bCanMove = true;
	}
}

void AArcherWSkillInstance::SpawnSkill()
{
	if (SkillActorClass == nullptr) return;

	{
		Protocol::C_SKILL Pkt;
		Pkt.set_skillid(1);
		Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

		Pkt.set_x(SkillPos.X);
		Pkt.set_y(SkillPos.Y);
		Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

		ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

		SEND_PACKET(Pkt);
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->bCanMove = false;
	}

	Cast<AP1Character>(OwnerCreature)->StopMoving();
}

void AArcherWSkillInstance::UseSkillAfterTargetingWithPos(FVector TargetPos)
{
	SkillPos = TargetPos;

	if (OwnerCreature)
	{
		OwnerCreature->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(this, &AArcherWSkillInstance::OnMontageEnded);
	}

	if (!IsValid(HoldingByTickSkillManager))
	{
		HoldingByTickSkillManager = Cast<AHoldingByTickSkillManager>(OwnerCreature->GetWorld()->SpawnActor(AHoldingByTickSkillManager::StaticClass()));
		HoldingByTickSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
		HoldingByTickSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
		HoldingByTickSkillManager->StartCasting(SkillInfo.CastingTime);
	}
	else
	{
		HoldingByTickSkillManager->StartCasting(SkillInfo.CastingTime);
		HoldingByTickSkillManager = nullptr;
		if (IsValid(CurrentSkillActor))
		{
			CurrentSkillActor->Destroy();
			CurrentSkillActor = nullptr;
		}
	}
}

void AArcherWSkillInstance::ActivateSkill(ASkillActorBase* SkillActor)
{
	CurrentSkillActor = SkillActor;
	// SkillActor->SetActorLocation(SkillPos);
}

void AArcherESkillInstance::UseSkill()
{
	Super::UseSkill();
	if (!IsValid(CastingSkillManager))
	{
		CastingSkillManager = Cast<ACastingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(ACastingSkillManager::StaticClass()));
		CastingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
		CastingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
		CastingSkillManager->StartCasting(SkillInfo.CastingTime);
	}
	else
	{
		CastingSkillManager->StartCasting(SkillInfo.CastingTime);
		CastingSkillManager = nullptr;
	}
}

void AArcherESkillInstance::SpawnSkill()
{
	if (SkillActorClass == nullptr) return;

	{
		Protocol::C_SKILL Pkt;
		Pkt.set_skillid(2);

		Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

		FVector SpawnLocation = OwnerCreature->GetActorLocation() + OwnerCreature->GetActorForwardVector() * 600;
		Pkt.set_x(SpawnLocation.X);
		Pkt.set_y(SpawnLocation.Y);
		Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

		ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

		SEND_PACKET(Pkt);
	}
}

void AArcherESkillInstance::ActivateSkill(ASkillActorBase* SkillActor)
{
	SkillActor->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	CurrentSkillActor = SkillActor;
}

void AArcherESkillInstance::OnCastingEnd()
{
	{
		Protocol::C_MONTAGE Pkt;
		Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
		CasterInfo->CopyFrom(*OwnerCreature->ObjectInfo);
		Pkt.set_isstop(false);
		Pkt.set_id(2);
		SEND_PACKET(Pkt);
	}

	if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->bCanMove = false;
	}
}

void AArcherESkillInstance::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if ((AnimMontage == M_Skill) && IsValid(CurrentSkillActor))
	{
		CurrentSkillActor->Destroy();
		CurrentSkillActor = nullptr;
		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->bCanMove = true;
		}
	}
}
