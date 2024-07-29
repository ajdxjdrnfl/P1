// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponentBase.h"
#include "P1/P1GameInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
//#include "P1/Skill/Manager/CastingSkillManager.h"
//#include "P1/Skill/Manager/ChargingSkillManager.h"
#include "P1/Skill/Warrior/WarriorSkillInstance.h"
#include "P1/P1GameMode.h"

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

			SkillInstances[2] = Cast<ASkillInstanceBase>(NewObject<AWarriorESkillInstance>());

			SkillInstances[3] = Cast<ASkillInstanceBase>(NewObject<AWarriorRSkillInstance>());
		}
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

	for (int i = 0; i < Skills.Num(); i++)
	{
		SkillInstances[i]->SetSkillActorClass(Skills[i].SkillActorClass);
		SkillInstances[i]->SetSkillAnim(Skills[i].AnimMontage);
		SkillInstances[i]->Init(Cast<AP1Character>(GetOwner()));
	}
}

void USkillComponentBase::UseSkill(uint16 SkillIndex)
{
	if (Skills.Num() <= SkillIndex || SkillInstances.Num() <= SkillIndex) return;

	CurrentSkillInfo = Skills[SkillIndex];
	CurrentSkillActor = Cast<ASkillActorBase>(CurrentSkillInfo.SkillActorClass->GetDefaultObject());
	AP1Creature* P1Creature = Cast<AP1Creature>(GetOwner());

	if (CurrentSkillActor == nullptr || OwnerAnimInstance == nullptr || P1Creature == nullptr)
		return;

	if (CurrentSkillInfo.AnimMontage == nullptr) 
		return;

	SkillInstances[SkillIndex]->SetSkillInfo(Skills[SkillIndex]);
	SkillInstances[SkillIndex]->UseSkill();
	

	// Normal Skill && Projectile Skill
	/*if (CurrentSkillActor->SkillInfo->skill_type() == Protocol::SKILL_TYPE_NORMAL)
	{
		Protocol::C_SKILL Pkt;
		Protocol::SkillInfo* SkillInfoRef = Pkt.mutable_skillinfo();
		Protocol::ObjectInfo* ObjectInfoRef = Pkt.mutable_caster();

		SkillInfoRef->CopyFrom(*CurrentSkillActor->SkillInfo);
		ObjectInfoRef->CopyFrom(*P1Creature->ObjectInfo);

		SkillState = ESkillState::Normal;
		
		SEND_PACKET(Pkt);
	}
	else if (CurrentSkillActor->SkillInfo->skill_type() == Protocol::SKILL_TYPE_CASTING)
	{
		if (CastingSkillManager == nullptr)
			CastingSkillManager = NewObject<UCastingSkillManager>();

		if (CurrentSkillActor != CastingSkillManager->GetSkillActor())
		{
			CastingSkillManager->SetSkillActor(CurrentSkillActor);
			CastingSkillManager->Init(Cast<AP1Character>(GetOwner()), CurrentSkillInfo);
		}
		
		CastingSkillManager->UseSkill();
	}
	else if (CurrentSkillActor->SkillInfo->skill_type() == Protocol::SKILL_TYPE_CHARGING)
	{
		if (ChargingSkillManager == nullptr)
			ChargingSkillManager = NewObject<UChargingSkillManager>();

		if (CurrentSkillActor != ChargingSkillManager->GetSkillActor())
		{
			ChargingSkillManager->SetSkillActor(CurrentSkillActor);
			ChargingSkillManager->Init(Cast<AP1Character>(GetOwner()), CurrentSkillInfo);
		}
		
		ChargingSkillManager->UseSkill();
	}*/
}

