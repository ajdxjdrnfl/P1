// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkillComponent.h"
#include "Engine/DataTable.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1.h"

void UCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SetSkills();
}

void UCharacterSkillComponent::UseSkill(uint16 SkillIndex)
{
	// TODO: Skills[SkillIndex];
	FGeneralSkillInfo GeneralSkillInfo = Skills[SkillIndex];

	if (OwnerAnimInstance == nullptr)
		return;

	//if (SkillInfo.AnimMontage == nullptr) 
	//	return;

	FVector Loc = GetOwner()->GetActorLocation();
	FRotator Rot = GetOwner()->GetActorRotation();
	ASkillActorBase* SkillActor = Cast<ASkillActorBase>(GetWorld()->SpawnActor(GeneralSkillInfo.SkillActor, &Loc, &Rot));
	if (SkillActor == nullptr)
		return;

	{
		Protocol::C_SKILL Pkt;
		Protocol::SkillInfo* SkillInfo = Pkt.mutable_skillinfo();
		SkillInfo->set_skill_id(1);
		SEND_PACKET(Pkt);
	}

	SkillActor->ActivateSkill();
}

