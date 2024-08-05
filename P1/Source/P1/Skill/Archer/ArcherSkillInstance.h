// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillInstanceBase.h"
#include "ArcherSkillInstance.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AArcherQSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class AChargingSkillManager* ChargingSkillManager;

public:
	virtual void UseSkill() override;
	virtual void SpawnSkill() override;
};
