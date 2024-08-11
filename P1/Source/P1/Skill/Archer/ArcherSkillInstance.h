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
	class ACastingSkillManager* CastingSkillManager;

public:
	virtual void UseSkill() override;
	virtual void SpawnSkill() override;
	virtual void OnCastingEnd() override;
};

UCLASS()
class P1_API AArcherWSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class AHoldingByTickSkillManager* HoldingByTickSkillManager;

	UPROPERTY()
	class ASkillActorBase* CurrentSkillActor;

public:
	virtual void UseSkill() override;
	virtual void SpawnSkill() override;
	virtual void ActivateSkill(ASkillActorBase* SkillActor);
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted) override;
};
