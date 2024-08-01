// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillManagerBase.h"
#include "P1/P1Character.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"

// Sets default values
ASkillManagerBase::ASkillManagerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

ESkillType ASkillManagerBase::GetSkillState()
{
	return OwnerCharacter->GetSkillComponent()->GetSkillState();
}

void ASkillManagerBase::SetSkillState(ESkillType StateToSet)
{
	OwnerCharacter->GetSkillComponent()->SetSkillState(StateToSet);
}

void ASkillManagerBase::Init(AP1Character* _OwnerCharacter, ASkillInstanceBase* _SkillInstance, FSkillInfo _SkillInfo)
{
	OwnerCharacter = _OwnerCharacter;
	SkillInstance = _SkillInstance;
	SkillInfo = _SkillInfo;
}


