// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillManagerSubSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillGaugeEnd);

UCLASS()
class P1_API USkillManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	TMap<uint16, bool> SkillCanUseMapByCooldownTime;
	TMap<uint16, bool> SkillCanUseMapByCasting;
	FOnSkillGaugeEnd OnSkillGaugeEnd;

	float CastingSkillGaugeRate;

	bool bCanMove = true;
	bool bCanUseSkill = true;
	bool bCanMoveByAnimMontage = true;

	FVector MouseLocation;

	UPROPERTY()
	class ASkillTargetingBase* CurrentSkillTargeting;

	FVector TargetingSkillLocation;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void SetKeyCanUse(int32 SkillIndex);
	bool CanUseSkill(int32 SkillIndex);
	bool CanMove();
	void CheckSkillTargeting();
};
