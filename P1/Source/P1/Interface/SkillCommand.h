// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "P1/Data/SkillData.h"
#include "SkillCommand.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USkillCommand : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class P1_API ISkillCommand
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void UseSkill() = 0;
	virtual void InitOnSpawn(class AP1Creature* OwnerCreature, const FSkillInfo& CurrentSkillInfo) = 0;
	virtual void ActivateSkill(class ASkillActorBase* SkillActor) = 0;
	virtual void SpawnSkill() = 0;
};
