// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillInstanceBase.h"
#include "MobSkillInstance.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AMobQSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()

public:
	virtual void UseSkill() override;
	virtual void SpawnSkill() override;
	
};
