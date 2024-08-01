// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillManagerBase.h"
#include "HoldingSkillManager.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AHoldingSkillManager : public ASkillManagerBase
{
	GENERATED_BODY()

private:
	float AttackTime = 0.5f;
	float CurrentTime;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void StartCasting(float CastingTime) override;
	void EndSkill();
	virtual void OnCastingEnd() override;
};
