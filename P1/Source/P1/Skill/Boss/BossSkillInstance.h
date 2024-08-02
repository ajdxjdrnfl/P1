// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillInstanceBase.h"
#include "BossSkillInstance.generated.h"

// 기본 공격
UCLASS()
class P1_API ABossQSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()
	
public:
	virtual void SpawnSkill() override;
	virtual void UseSkill() override;
};

// 돌진
UCLASS()
class P1_API ABossWSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()

public:
	virtual void SpawnSkill() override;
	virtual void UseSkill() override;
};
