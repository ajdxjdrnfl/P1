// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponentBase.h"
#include "P1/P1GameInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"

USkillComponentBase::USkillComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void USkillComponentBase::BeginPlay()
{
	Super::BeginPlay();

	if (UP1GameInstance* GameInstance = Cast<UP1GameInstance>(GetWorld()->GetGameInstance()))
	{
		SkillDataTable = GameInstance->SkillDataTable;
	}
	
}


void USkillComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillComponentBase::SetSkills()
{
	if (SkillDataTable == nullptr) return;
	FString ContextString;
	FSkillsByClass SkillsByClass = *SkillDataTable->FindRow<FSkillsByClass>(/* TODO: */ FName("Warrior"), ContextString);
	Skills = SkillsByClass.SkillInfos;
}

void USkillComponentBase::UseSkill(uint16 SkillIndex)
{
	// TODO: Skills[SkillIndex];
	FGeneralSkillInfo GeneralSkillInfo = Skills[SkillIndex];

	if (OwnerAnimInstance == nullptr)
		return;

	//if (SkillInfo.AnimMontage == nullptr) 
	//	return;

	UP1GameInstance* P1GameInstace = GetWorld()->GetGameInstance<UP1GameInstance>();
	if (P1GameInstace == nullptr) return;

	FSkillActorInfo SkillActorInfo = Cast<ASkillActorBase>(GeneralSkillInfo.SkillActor->GetDefaultObject())->GetSkillInfo();

	{
		Protocol::C_SKILL Pkt;
		Protocol::SkillInfo* SkillInfo = Pkt.mutable_skillinfo();
		Protocol::ObjectInfo* ObjectInfo = Pkt.mutable_caster();

		AP1Creature* P1Creature = Cast<AP1Character>(GetOwner());
		if (P1Creature == nullptr)
			return;

		SkillInfo->set_skill_id(1);
		SkillInfo->set_size_x(SkillActorInfo.XScale);
		SkillInfo->set_size_y(SkillActorInfo.YScale);
		SkillInfo->set_damage(SkillActorInfo.DamageInfo.Damage);
		ObjectInfo->CopyFrom(*P1Creature->Info);

		switch (SkillActorInfo.CollisionType)
		{
		case ECollisionType::Circle:
			SkillInfo->set_collision_type(Protocol::CollisionType::COLLISION_TYPE_CIRCLE);
			break;
		case ECollisionType::Box:
			SkillInfo->set_collision_type(Protocol::CollisionType::COLLISION_TYPE_BOX);
			break;
		default:
			break;
		}

		switch (SkillActorInfo.DamageInfo.DamageType)
		{
		case EDamageType::Normal:
			SkillInfo->set_damage_type(Protocol::DamageType::DAMAGE_TYPE_NORMAL);
			break;
		case EDamageType::Dot:
			SkillInfo->set_damage_type(Protocol::DamageType::DAMAGE_TYPE_DOT);
			break;
		case EDamageType::Buff:
			SkillInfo->set_damage_type(Protocol::DamageType::DAMAGE_TYPE_BUFF);
			break;
		default:
			break;
		}

		switch (SkillActorInfo.DamageInfo.CCType)
		{
		case ECCType::Normal:
			SkillInfo->set_cc_type(Protocol::CCType::CC_TYPE_NORMAL);
			break;
		case ECCType::Slow:
			SkillInfo->set_cc_type(Protocol::CCType::CC_TYPE_SLOW);
			break;
		case ECCType::Stun:
			SkillInfo->set_cc_type(Protocol::CCType::CC_TYPE_STUN);
			break;
		case ECCType::Airborne:
			SkillInfo->set_cc_type(Protocol::CCType::CC_TYPE_AIRBORNE);
			break;
		default:
			break;
		}

		SEND_PACKET(Pkt);
	}

	//SkillActor->ActivateSkill();
}

