// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/Manager/HoldingSkillManager.h"
#include "HoldingByTickSkillManager.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AHoldingByTickSkillManager : public AHoldingSkillManager
{
	GENERATED_BODY()

private:
	bool bIsActivated;
	float AttackTime = 0.5f;
	float CurrentTime;

public:
	virtual void StartCasting(float CastingTime) override;
};
