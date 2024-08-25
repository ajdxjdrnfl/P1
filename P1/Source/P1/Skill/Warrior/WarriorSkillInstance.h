// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/SkillInstanceBase.h"
#include "WarriorSkillInstance.generated.h"

/**
 *
 */
UCLASS()
class P1_API AWarriorQSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()

public:
	AWarriorQSkillInstance();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bDoOnceActivated;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsComboTiming;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsTurning;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	int32 AttackIndex;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FName AttackSectionIndex;

public:
	virtual void SpawnSkill() override;
	virtual void UseSkill() override;
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted) override;

};

UCLASS()
class P1_API AWarriorWSkillInstance : public ASkillInstanceBase
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
	virtual void ActivateSkill(class ASkillActorBase* SkillActor) override;
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted) override;
};

UCLASS()
class P1_API AWarriorESkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class AChargingSkillManager* ChargingSkillManager;

public:
	virtual void UseSkill() override;
	virtual void SpawnSkill() override;
	virtual void ActivateSkill(class ASkillActorBase* SkillActor) override;
};

UCLASS()
class P1_API AWarriorRSkillInstance : public ASkillInstanceBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class ACastingSkillManager* CastingSkillManager;

	UPROPERTY()
	class ASkillActorBase* CurrentSkillActor;

	UPROPERTY()
	class AOwnerToCursorSkillTargeting* SkillTargeting;

public:
	virtual void Init(class AP1Creature* _OwnerCreature);
	virtual void UseSkill() override;
	virtual void UseSkillAfterTargeting() override;
	void SetAnimMontage();
	virtual void SpawnSkill() override;
	virtual void OnCastingEnd() override;
	virtual void ActivateSkill(class ASkillActorBase* SkillActor) override;
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte) override;
};
