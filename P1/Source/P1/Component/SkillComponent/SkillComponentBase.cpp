// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponentBase.h"
#include "P1/P1GameInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/Skill/Manager/CastingSkillManager.h"

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
	CurrentSkillInfo = Skills[SkillIndex];
	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(CurrentSkillInfo.SkillActorClass->GetDefaultObject());
	AP1Creature* P1Creature = Cast<AP1Creature>(GetOwner());

	if (SkillActor == nullptr || OwnerAnimInstance == nullptr || P1Creature == nullptr)
		return;
	//if (SkillInfo.AnimMontage == nullptr) 
	//	return;

	// Normal Skill && Projectile Skill
	if (SkillActor->SkillInfo->skill_type() == Protocol::SKILL_TYPE_NORMAL)
	{
		Protocol::C_SKILL Pkt;
		Protocol::SkillInfo* SkillInfoRef = Pkt.mutable_skillinfo();
		Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

		SkillInfoRef->CopyFrom(*SkillActor->SkillInfo);
		ObjectInfoRef->CopyFrom(*P1Creature->ObjectInfo);

		SkillState = ESkillState::Normal;
		
		SEND_PACKET(Pkt);
	}
	else if (SkillActor->SkillInfo->skill_type() == Protocol::SKILL_TYPE_CASTING)
	{
		if (CastingSkillManager == nullptr)
		{
			CastingSkillManager = NewObject<UCastingSkillManager>();
			CastingSkillManager->Init(Cast<AP1Character>(GetOwner()), CurrentSkillInfo);
		}
		CastingSkillManager->UseSkill();
	}
}

