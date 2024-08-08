// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Data/SkillData.h"
#include "SkillInstanceBase.generated.h"

UCLASS()
class P1_API ASkillInstanceBase : public AActor
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
	void Init(class AP1Creature* _OwnerCreature);

	UFUNCTION(BlueprintCallable)
	virtual void SpawnSkill() {};

	UFUNCTION(BlueprintCallable)
	virtual void SpawnSkillAtLocation(FVector2D Location) {};

	UFUNCTION(BlueprintCallable)
	virtual void UseSkill() {};

	UFUNCTION(BlueprintCallable)
	virtual void UseSkillAfterTargeting() {};

	UFUNCTION(BlueprintCallable)
	virtual void ActivateSkill(class ASkillActorBase* SkillActor);

	UFUNCTION(BlueprintCallable)
	virtual void StartMontage(int32 SkillIndexLocal);

	UFUNCTION(BlueprintCallable)
	virtual void StopMontage(int32 SkillIndexLocal);

	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);

	UFUNCTION()
	virtual void OnSkillDetroyed(AActor* ActorToDestroy) {}

	virtual void OnCastingEnd() {}

	UFUNCTION(BlueprintCallable)
	virtual void SetSkillInstanceOfSkill(class ASkillActorBase* SkillActor);

	FORCEINLINE void SetSkillInfo(FSkillInfo SkillInfoToSet) { SkillInfo = SkillInfoToSet; }
	FORCEINLINE void SetSkillAnim(class UAnimMontage* M_Montage) { M_Skill = M_Montage; }
	FORCEINLINE void SetSkillActorClass(TSubclassOf<class ASkillActorBase> _SkillActorClass) { SkillActorClass = _SkillActorClass; }

};
