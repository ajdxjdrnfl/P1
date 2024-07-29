// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillActorBase.h"
#include "WarriorSkills.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AWarriorQSkill : public ASkillActorBase
{
	GENERATED_BODY()

public:
	AWarriorQSkill();

private:
	UPROPERTY()
	class AP1Character* OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UBoxComponent* BoxCollision;

public:
	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

};

UCLASS()
class P1_API AWarriorWSkill : public ASkillActorBase
{
	GENERATED_BODY()

public:
	AWarriorWSkill();

private:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* M_Skill;

public:
	virtual void ActivateSkill() override;

};

UCLASS()
class P1_API AWarriorESkill : public ASkillActorBase
{
	GENERATED_BODY()

public:
	AWarriorESkill();

private:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* M_Skill;

public:
	virtual void ActivateSkill() override;

};

UCLASS()
class P1_API AWarriorRSkill : public ASkillActorBase
{
	GENERATED_BODY()

public:
	AWarriorRSkill();

private:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* M_Skill;

public:
	virtual void ActivateSkill() override;

};
