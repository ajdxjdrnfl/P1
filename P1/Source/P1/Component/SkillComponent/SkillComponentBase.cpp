// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponentBase.h"
#include "P1/P1GameInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"
#include "P1/Skill/Warrior/WarriorSkillInstance.h"
#include "P1/Skill/Boss/BossSkillInstance.h"
#include "P1/P1GameMode.h"

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
		else if (P1Creature->GetClassType() == FName("Boss"))
		{
			SkillInstances[0] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossQSkillInstance::StaticClass()));
			SkillInstances[0]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);

			SkillInstances[1] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossWSkillInstance::StaticClass()));
			SkillInstances[1]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);

			/*SkillInstances[2] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossESkillInstance::StaticClass()));
			SkillInstances[2]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);

			SkillInstances[3] = Cast<ASkillInstanceBase>(P1Creature->GetWorld()->SpawnActor(ABossRSkillInstance::StaticClass()));
			SkillInstances[3]->AttachToActor(P1Creature, FAttachmentTransformRules::KeepWorldTransform);*/
		}
		else if (P1Creature->GetClassType() == FName("Mob"))
		{
			return;
		}
	}

	FName ClassTypeName = Cast<AP1Creature>(GetOwner())->GetClassType();
	if (ClassTypeName == NAME_None) return;
	FString ContextString;
	FSkillsByClass SkillsByClass = *SkillDataTable->FindRow<FSkillsByClass>(ClassTypeName, ContextString);
	Skills = SkillsByClass.SkillInfos;

	for (int i = 0; i < Skills.Num(); i++)
	{
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

	SkillInstances[SkillIndex]->SetSkillInfo(Skills[SkillIndex]);
	SkillInstances[SkillIndex]->UseSkill();
}

