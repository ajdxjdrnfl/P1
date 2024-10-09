// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Data/SkillData.h"
#include "P1/Interface/SkillCommand.h"
#include "SkillInstanceBase.generated.h"

UCLASS()
class P1_API ASkillInstanceBase : public AActor, public ISkillCommand
{
	GENERATED_BODY()

public:
	ASkillInstanceBase();

protected:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class UAnimMontage* M_Skill;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<class ASkillActorBase> SkillActorClass;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class AP1Creature* OwnerCreature;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class ASkillActorBase* SpawnedSkillActor;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FSkillInfo SkillInfo;

public:
	// Call after click key
	UFUNCTION(BlueprintCallable)
	virtual void UseSkill() override;

	UFUNCTION(BlueprintCallable)
	virtual void InitOnSpawn(class AP1Creature* _OwnerCreature, const FSkillInfo& SkillInfoToSet) override;

	void Init(class AP1Creature* _OwnerCreature);

	// Call when spawn skill
	UFUNCTION(BlueprintCallable)
	virtual void SpawnSkill() override {};

	// If using targeting skill actor, call after click targeting skill
	UFUNCTION(BlueprintCallable)
	virtual void UseSkillAfterTargeting() {};

	UFUNCTION(BlueprintCallable)
	virtual void UseSkillAfterTargetingWithPos(FVector TargetPos) {};

	// Call after spawn skill
	UFUNCTION(BlueprintCallable)
	virtual void ActivateSkill(class ASkillActorBase* SkillActor);

	UFUNCTION(BlueprintCallable)
	virtual void SpawnSkillAtLocation(FVector2D Location) {};

	UFUNCTION(BlueprintCallable)
	virtual void StartMontage(int32 SkillIndexLocal);

	UFUNCTION(BlueprintCallable)
	virtual void StopMontage(int32 SkillIndexLocal);

	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);

	UFUNCTION()
	virtual void OnSkillDestroyed(AActor* ActorToDestroy) {}

	virtual void OnCastingEnd() {}

	UFUNCTION(BlueprintCallable)
	virtual void SetSkillInstanceOfSkill(class ASkillActorBase* SkillActor);

};
