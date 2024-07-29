// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillManagerBase.h"
#include "P1/Data/SkillData.h"
#include "CastingSkillManager.generated.h"

/**
 * 
 */
UCLASS()
class P1_API ACastingSkillManager : public ASkillManagerBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	void Init(class AP1Character* _OwnerCharacter, class ASkillInstanceBase* _SkillInstance, FSkillInfo _SkillInfo);
	void StartCasting();

	UFUNCTION()
	void OnCastingEnd();

};
