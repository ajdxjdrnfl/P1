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
	void Init(class AP1Character* _OwnerCharacter, class ASkillInstanceBase* _SkillInstance, FSkillInfo _SkillInfo);
	void StartCasting();
	void FireSkill(float rate);

	UFUNCTION()
	void OnCastingEnd();

	FORCEINLINE class ASkillActorBase* GetSkillActor() const { return SkillActor; }
	FORCEINLINE void SetSkillActor(ASkillActorBase* SkillActorToSet) { SkillActor = SkillActorToSet; }

};
