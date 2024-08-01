// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillManagerBase.h"
#include "P1/Data/SkillData.h"
#include "ChargingSkillManager.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AChargingSkillManager : public ASkillManagerBase
{
	GENERATED_BODY()

public:
	virtual void StartCasting(float CastingTime) override;
	void FireSkill(float rate);

	virtual void OnCastingEnd() override;

	FORCEINLINE class ASkillActorBase* GetSkillActor() const { return SkillActor; }
	FORCEINLINE void SetSkillActor(ASkillActorBase* SkillActorToSet) { SkillActor = SkillActorToSet; }

};
