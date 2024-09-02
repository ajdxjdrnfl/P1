// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponentBase.h"
#include "P1/P1GameInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/Skill/Warrior/WarriorSkillInstance.h"
#include "P1/Skill/Boss/BossSkillInstance.h"
#include "P1/Skill/Mob/MobSkillInstance.h"
#include "P1/Skill/Archer/ArcherSkillInstance.h"
#include "P1/P1GameMode.h"
#include "Kismet/KismetMathLibrary.h"

USkillComponentBase::USkillComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void USkillComponentBase::BeginPlay()
{
	Super::BeginPlay();

}


void USkillComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillComponentBase::InitOnSpawn()
{
	SetSkills();

}

void USkillComponentBase::SetSkills()
{
	if (UP1GameInstance* GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance()))
	{
		SkillDataTable = GameInstance->SkillDataTable;
	}

	AP1Creature* P1Creature = Cast<AP1Creature>(GetOwner());
	if (P1Creature)
	{
		SkillInstances.SetNum(4);
		if (P1Creature->GetClassType() == FName("Warrior"))
		{
			SkillInstances[0] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AWarriorQSkillInstance::StaticClass()));
			SkillInstances[0]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[1] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AWarriorWSkillInstance::StaticClass()));
			SkillInstances[1]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[2] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AWarriorESkillInstance::StaticClass()));
			SkillInstances[2]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[3] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AWarriorRSkillInstance::StaticClass()));
			SkillInstances[3]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
		}
		else if (P1Creature->GetClassType() == FName("Archer"))
		{
			SkillInstances[0] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AArcherQSkillInstance::StaticClass()));
			SkillInstances[0]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[1] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AArcherWSkillInstance::StaticClass()));
			SkillInstances[1]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[2] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AArcherESkillInstance::StaticClass()));
			SkillInstances[2]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[3] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AArcherRSkillInstance::StaticClass()));
			SkillInstances[3]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);

		}
		else if (P1Creature->GetClassType() == FName("Boss"))
		{
			SkillInstances[0] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossQSkillInstance::StaticClass()));
			SkillInstances[0]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[1] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossWSkillInstance::StaticClass()));
			SkillInstances[1]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[2] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossESkillInstance::StaticClass()));
			SkillInstances[2]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
			SkillInstances[3] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossRSkillInstance::StaticClass()));
			SkillInstances[3]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
		}
		else if (P1Creature->GetClassType() == FName("Mob"))
		{
			SkillInstances[0] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(AMobQSkillInstance::StaticClass()));
			SkillInstances[0]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);
		}
	}

	FName ClassTypeName = Cast<AP1Creature>(GetOwner())->GetClassType();
	if (ClassTypeName == NAME_None) return;
	FString ContextString;
	FSkillsByClass SkillsByClass = *SkillDataTable->FindRow<FSkillsByClass>(ClassTypeName, ContextString);
	Skills = SkillsByClass.SkillInfos;

	for (int i = 0; i < Skills.Num(); i++)
	{
		SkillInstances[i]->SetSkillInfo(Skills[i]);
		SkillInstances[i]->SetSkillActorClass(Skills[i].SkillActorClass);
		SkillInstances[i]->SetSkillAnim(Skills[i].AnimMontage);
		SkillInstances[i]->Init(Cast<AP1Creature>(GetOwner()));
	}
}

FSkillInfo USkillComponentBase::GetSkillInfoByIndex(int32 SkillIndex)
{
	return Skills[SkillIndex];
}

void USkillComponentBase::UseSkill(int32 SkillIndex)
{
	if (Skills.Num() <= SkillIndex || SkillInstances.Num() <= SkillIndex) return;

	CurrentSkillInfo = Skills[SkillIndex];

	if (CurrentSkillInfo.AnimMontage == nullptr) 
		return;

	// SkillInstances[SkillIndex]->SetSkillInfo(Skills[SkillIndex]);
	SkillInstances[SkillIndex]->UseSkill();
}

void USkillComponentBase::PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt)
{
	UAnimInstance* AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance();
	UAnimMontage* AnimMontage = Skills[pkt.id()].AnimMontage;
	if (AnimInstance == nullptr || AnimMontage == nullptr) return;

	if (pkt.section_num() > 0)
	{
		if (!AnimInstance->Montage_IsPlaying(AnimMontage))
		{
			PlayAnimMontageByDuration(AnimInstance, AnimMontage, pkt);
		}

		FString Str = FString::FromInt(pkt.section_num());
		FName SectionIndexName = FName(*Str);
		AnimInstance->Montage_JumpToSection(SectionIndexName);
		return;
	}

	

	if (!pkt.isstop())
	{
		PlayAnimMontageByDuration(AnimInstance, AnimMontage, pkt);
	}
	else
	{
		AnimInstance->Montage_Stop(0.3f, AnimMontage);
	}
}

void USkillComponentBase::PlayAnimMontageByDuration(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage, Protocol::S_MONTAGE& pkt)
{
	float PlayRate = 1;
	if (pkt.scalable())
	{
		PlayRate = AnimMontage->GetSectionLength(pkt.section_num()) / pkt.duration();

	}
	AnimInstance->Montage_Play(AnimMontage, PlayRate);
}

void USkillComponentBase::PlayAnimMontageByDurationNotPacket(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage, int32 SectionNum, float TimeDuration)
{
	float PlayRate = AnimMontage->GetSectionLength(SectionNum / TimeDuration);
	AnimInstance->Montage_Play(AnimMontage, PlayRate);
}

void USkillComponentBase::SetSpawnedSkill(int32 SkillID, ASkillActorBase* SkillActor)
{
	if (SkillInstances[SkillID] == nullptr) return;
	SkillInstances[SkillID]->ActivateSkill(SkillActor);
}

void USkillComponentBase::SetCC(ASkillActorBase* SkillActor)
{
	if (!IsValid(SkillActor)) return;
	if (SkillActor->DoOnceCCSet.Contains(OwnerCreature)) return;
	SkillActor->DoOnceCCSet.Add(OwnerCreature);

	if (OwnerCreature->ObjectInfo->state() != Protocol::MOVE_STATE_DEAD)
	{
		if (!OwnerCreature->GetMesh()->GetAnimInstance()->Montage_IsPlaying(nullptr))
		{
			OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Stop(0.3f);
		}

		if (SkillActor->SkillInfo.CCType == ECCType::NockBack)
		{
			OwnerCreature->ObjectInfo->set_state(Protocol::MOVE_STATE_STUN);
			ProcKnockBack(SkillActor);
		}
		else if (SkillActor->SkillInfo.CCType == ECCType::Stun)
		{
			OwnerCreature->ObjectInfo->set_state(Protocol::MOVE_STATE_STUN);
			ProcStun();
		}
	}
}

void USkillComponentBase::ProcKnockBack(ASkillActorBase* SkillActor)
{
	OwnerCreature->GetKnockBack(SkillActor->SkillInfo.CCTime);

	FVector2D CharacterLocation = FVector2D(OwnerCreature->GetActorLocation().X, OwnerCreature->GetActorLocation().Y);
	FVector2D SkillLocation = FVector2D(SkillActor->GetActorLocation().X, SkillActor->GetActorLocation().Y);
	float CrossResult = UKismetMathLibrary::CrossProduct2D(CharacterLocation, SkillLocation);

	if (OwnerCreature->GetMesh()->GetAnimInstance()->Montage_IsPlaying(M_KnockBack_L) ||
		OwnerCreature->GetMesh()->GetAnimInstance()->Montage_IsPlaying(M_KnockBack_R))
		return;

	if (CrossResult > 0)
	{
		OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Play(M_KnockBack_R);
	}
	else
	{
		OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Play(M_KnockBack_L);
	}
}

void USkillComponentBase::ProcStun()
{
	PlayAnimMontageByDurationNotPacket(OwnerCreature->GetMesh()->GetAnimInstance(), M_Stun, 1, 2);
	OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Play(M_Stun);
}
