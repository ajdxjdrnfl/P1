// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillInstanceBase.h"
#include "BossSkillInstance.generated.h"

/**
 * 
 */
UCLASS()
class P1_API ABossQSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()
	
public:
	virtual void SpawnSkill() override;
	virtual void UseSkill() override;
};
