// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorSkillInstance.h"
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
#include "Particles/ParticleSystemComponent.h"
#include "P1/Skill/Targeting/OwnerToCursorSkillTargeting.h"

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
	bDoOnceActivated = true; // set false at skill end anim notify

	{
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

void AWarriorQSkillInstance::UseSkill()
{
	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(nullptr);
	if (!bIsMontagePlaying)
	{
		AttackIndex = 1;
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->set_object_id(OwnerCreature->ObjectInfo->object_id());
			Pkt.set_isstop(false);
			Pkt.set_id(0);

			SEND_PACKET(Pkt);
		}

		OwnerCreature->ObjectInfo->set_state(Protocol::MOVE_STATE_SKILL);
		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->SetKeyCanUse(SkillInfo.SkillNumLocal);
		}
	}
	else if (bIsComboTiming)
	{
		AttackIndex++;
		bIsComboTiming = false;
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->CopyFrom(*OwnerCreature->ObjectInfo);
			Pkt.set_isstop(false);
			Pkt.set_id(0);
			Pkt.set_section_num(FCString::Atoi(*AttackSectionIndex.ToString()));

			SEND_PACKET(Pkt);

			OwnerCreature->ObjectInfo->set_state(Protocol::MOVE_STATE_IDLE);
		}
	}
}

void AWarriorWSkillInstance::UseSkill()
{
	if (OwnerCreature)
	{
		OwnerCreature->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(this, &AWarriorWSkillInstance::OnMontageEnded);
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
	}
}

void AWarriorWSkillInstance::SpawnSkill()
{
	if (!IsValid(CurrentSkillActor)) return;
	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(1);
	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	FVector SpawnLocation = OwnerCreature->GetActorLocation();
	Pkt.set_x(SpawnLocation.X);
	Pkt.set_y(SpawnLocation.Y);
	Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void AWarriorWSkillInstance::ActivateSkill(ASkillActorBase* SkillActor)
{
	SkillActor->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	CurrentSkillActor = SkillActor;
}

void AWarriorWSkillInstance::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if ((AnimMontage == M_Skill) && IsValid(CurrentSkillActor) && IsValid(HoldingByTickSkillManager))
	{
		CurrentSkillActor->Destroy();
		CurrentSkillActor = nullptr;
		HoldingByTickSkillManager->Destroy();
		HoldingByTickSkillManager = nullptr;
	}
}

void AWarriorESkillInstance::UseSkill()
{
	SpawnSkill();
}

void AWarriorESkillInstance::SpawnSkill()
{
	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(2);

	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	FVector SpawnLocation = OwnerCreature->GetActorLocation();
	Pkt.set_x(SpawnLocation.X);
	Pkt.set_y(SpawnLocation.Y);
	Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void AWarriorESkillInstance::ActivateSkill(ASkillActorBase* SkillActor)
{
	SkillActor->SpawnActivationParticleOnLocation(OwnerCreature->GetActorLocation());
}

void AWarriorRSkillInstance::Init(AP1Creature* _OwnerCreature)
{
	Super::Init(_OwnerCreature);

	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr)
		return;

	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &AWarriorRSkillInstance::OnMontageEnded);
}

void AWarriorRSkillInstance::UseSkill()
{
	if (!IsValid(SkillTargeting))
	{
		if (IsValid(CastingSkillManager))
		{
			UseSkillAfterTargeting();
			return;
		}

		SkillTargeting = Cast<AOwnerToCursorSkillTargeting>(OwnerCreature->GetWorld()->SpawnActor(SkillInfo.SkillTargeting));
		SkillTargeting->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
		SkillTargeting->Init(this, 0.5f);
	}
	else
	{
		SkillTargeting->Destroy();
		SkillTargeting = nullptr;
	}
}

void AWarriorRSkillInstance::UseSkillAfterTargeting()
{
	if (!IsValid(CastingSkillManager))
	{
		CastingSkillManager = Cast<ACastingSkillManager>(OwnerCreature->GetWorld()->SpawnActor(ACastingSkillManager::StaticClass()));
		CastingSkillManager->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
		CastingSkillManager->Init(Cast<AP1Character>(OwnerCreature), this, SkillInfo);
		CastingSkillManager->StartCasting(SkillInfo.CastingTime);
	}
	else
	{
		// To Stop Casting
		CastingSkillManager->StartCasting(SkillInfo.CastingTime);
		CastingSkillManager = nullptr;
	}
	SetAnimMontage();
}

void AWarriorRSkillInstance::SetAnimMontage()
{
	UAnimInstance* AnimInstance = OwnerCreature->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(M_Skill);
	if (!bIsMontagePlaying)
	{
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->CopyFrom(*OwnerCreature->ObjectInfo);
			Pkt.set_isstop(false);
			Pkt.set_id(3);

			SEND_PACKET(Pkt);
		}

		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->bCanMove = false;
		}
	}
	else
	{
		{
			Protocol::C_MONTAGE Pkt;
			Protocol::ObjectInfo* CasterInfo = Pkt.mutable_caster();
			CasterInfo->CopyFrom(*OwnerCreature->ObjectInfo);
			Pkt.set_isstop(true);
			Pkt.set_id(3);

			SEND_PACKET(Pkt);
		}

		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->bCanMove = true;
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
			CasterInfo->CopyFrom(*OwnerCreature->ObjectInfo);
			Pkt.set_isstop(false);
			Pkt.set_id(3);
			Pkt.set_section_num(2);

			SEND_PACKET(Pkt);
		}
	}

	if (SkillActorClass == nullptr) return;

	Protocol::C_SKILL Pkt;
	Pkt.set_skillid(3);

	Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

	FVector SpawnLocation = OwnerCreature->GetActorLocation();
	Pkt.set_x(SpawnLocation.X);
	Pkt.set_y(SpawnLocation.Y);
	Pkt.set_yaw(OwnerCreature->GetActorRotation().Yaw);

	ObjectInfoRef->CopyFrom(*OwnerCreature->ObjectInfo);

	SEND_PACKET(Pkt);
}

void AWarriorRSkillInstance::OnCastingEnd()
{
	SpawnSkill();
}

void AWarriorRSkillInstance::ActivateSkill(ASkillActorBase* SkillActor)
{
	SkillActor->AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepWorldTransform);
	CurrentSkillActor = SkillActor;
}

void AWarriorRSkillInstance::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte)
{
	if (AnimMontage == M_Skill)
	{
		if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->bCanMove = true;
		}
	}

	if (CurrentSkillActor)
	{
		CurrentSkillActor->Destroy();
		CurrentSkillActor = nullptr;
	}
}
