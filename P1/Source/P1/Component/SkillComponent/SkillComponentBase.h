// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P1/Data/SkillData.h"
#include "P1/P1.h"
#include "P1/Interface/SkillCommand.h"
#include "SkillComponentBase.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class P1_API USkillComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkillComponentBase();

protected:
	virtual void BeginPlay() override;

	friend class AP1Character;
	friend class AEnemyBase;

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FSkillInfo CurrentSkillInfo;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	class ASkillActorBase* CurrentSkillActor;

	/*UPROPERTY()
	class UCastingSkillManager* CastingSkillManager;

	UPROPERTY()
	class UChargingSkillManager* ChargingSkillManager;*/

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<TScriptInterface<ISkillCommand>> SkillCommands;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Editable, meta = (AllowPrivateAccess = true))
	class UAnimMontage* M_KnockBack_L;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Editable, meta = (AllowPrivateAccess = true))
	class UAnimMontage* M_KnockBack_R;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Editable, meta = (AllowPrivateAccess = true))
	class UAnimMontage* M_Stun;

	UPROPERTY()
	class AP1Creature* OwnerCreature;

public:
	TArray<FSkillInfo> Skills;

	ESkillType SkillState;

	UPROPERTY()
	class UDataTable* SkillDataTable;

	UPROPERTY()
	class UAnimInstance* OwnerAnimInstance;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitOnSpawn();
	void SetCC(class ASkillActorBase* SkillActor);
	void ProcKnockBack(class ASkillActorBase* SkillActor);
	void ProcStun();

public:
	void SetSkills();
	FSkillInfo GetSkillInfoByIndex(int32 SkillIndex);

	UFUNCTION(BlueprintCallable)
	virtual void UseSkill(int32 SkillIndex);
	virtual void PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt);
	void PlayAnimMontageByDuration(class UAnimInstance* AnimInstance, class UAnimMontage* AnimMontage, Protocol::S_MONTAGE& pkt);
	void PlayAnimMontageByDurationNotPacket(class UAnimInstance* AnimInstance, class UAnimMontage* AnimMontage, int32 SectionNum, float TimeDuration);
	void SetSpawnedSkill(int32 SkillID, class ASkillActorBase* SkillActor);
	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte) {}

	FORCEINLINE ESkillType GetSkillState() const { return SkillState; }
	FORCEINLINE void SetSkillState(ESkillType SkillStateToSet) { SkillState = SkillStateToSet; }
	FORCEINLINE TArray<TScriptInterface<ISkillCommand>> GetSkillInstances() { return SkillCommands; }
	FORCEINLINE UAnimMontage* GetKockBackLMontage() const { return M_KnockBack_L; }
	FORCEINLINE UAnimMontage* GetKockBackRMontage() const { return M_KnockBack_R; }
	FORCEINLINE UAnimMontage* GetStunMontage() const { return M_Stun; }
};
