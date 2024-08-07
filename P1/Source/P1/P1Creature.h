// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P1.h"
#include "P1/Data/SkillData.h"
#include "P1Creature.generated.h"

UENUM(BlueprintType)
enum class ECreatureState : uint8
{
	Idle UMETA(Displayname = "Idle"),
	Stun UMETA(Displayname = "Stun"),
	Move UMETA(Displayname = "Move"),
	Skill UMETA(Displayname = "Skill"),
	Max UMETA(Displayname = "Max"),
};

UCLASS()
class P1_API AP1Creature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AP1Creature();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY()
	class UP1ObjectBase* ObjectBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Editable, meta = (AllowPrivateAccess = true))
	FName ClassType;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsCounterState;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Editable, meta = (AllowPrivateAccess = true))
	class UAnimMontage* M_Die;

public:
	UPROPERTY(BlueprintReadWrite)
	ECreatureState CreatureState;

	Protocol::ObjectInfo* ObjectInfo;
	Protocol::ObjectInfo* TargetInfo;

	float CCTime;
	float DefaultWalkSpeed;

	virtual void Die();

	UFUNCTION(BlueprintCallable)
	void SetCreatureStateByServer(FSkillInfo SkillInfo);

	UFUNCTION(BlueprintCallable)
	void GetStun(float _CCTime);

	UFUNCTION(BlueprintCallable)
	int32 GetClassTypeInt();

	virtual void MoveByServer(float DeltaTime) {}
	void SetMoveValueByServer(Protocol::S_MOVE Pkt);

	void SetObjectInfo();
	virtual void SetSpawnedSkill(int32 SkillID, class ASkillActorBase* SkillActor) {}

	virtual void PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt) {}
	virtual void SetHealthByDamage(float HealthToSet) {}
	virtual FSkillInfo GetSkillInfoByIndex(int32 SkillIndex) { return FSkillInfo(); }
	void SetWalkSpeed(float WalkSpeed);

	FORCEINLINE class UP1ObjectBase* GetObjectBase() { return ObjectBase; }
	FORCEINLINE FName GetClassType() const { return ClassType; }
	FORCEINLINE void SetInfo(Protocol::ObjectInfo InfoToSet) { ObjectInfo->CopyFrom(InfoToSet); }
	FORCEINLINE bool GetCounterState() const { return bIsCounterState; }
};
