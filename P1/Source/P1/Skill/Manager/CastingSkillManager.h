// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "P1/Data/SkillData.h"
#include "CastingSkillManager.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UCastingSkillManager : public UObject
{
	GENERATED_BODY()

private:
	FSkillInfo CurrentSkillInfo;

	UPROPERTY()
	class AP1Character* OwnerCharacter;

	UPROPERTY()
	class ASkillActorBase* SkillActor;

public:
	void Init(class AP1Character* OwnerChar, FSkillInfo SkillInfo);
	void UseSkill();

	UFUNCTION()
	void OnCastingEnd();

	FORCEINLINE class ASkillActorBase* GetSkillActor() const { return SkillActor; }
	FORCEINLINE void SetSkillActor(ASkillActorBase* SkillActorToSet) { SkillActor = SkillActorToSet; }

};
