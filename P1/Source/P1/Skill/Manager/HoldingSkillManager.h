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
	bool bIsTick;
	bool bIsActivated;
	float AttackTime = 1;
	float CurrentTime;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void StartCasting(float CastingTime) override;
	virtual void EndSkill();
	virtual void OnCastingEnd() override;

	FORCEINLINE bool IsTick() const { return bIsTick; }
	FORCEINLINE void SetTick(bool bIsTickToSet) { bIsTick = bIsTickToSet; }

};
