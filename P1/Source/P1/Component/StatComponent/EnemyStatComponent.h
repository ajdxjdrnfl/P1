// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/StatComponent/StatComponentBase.h"
#include "P1/Data/SkillData.h"
#include "EnemyStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyStaminaChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHealthChangedDelegate);

UCLASS()
class P1_API UEnemyStatComponent : public UStatComponentBase
{
	GENERATED_BODY()

public:
	friend class AEnemyBase;

private:
	UPROPERTY()
	class AEnemyBase* OwnerEnemy;

public:
	FOnEnemyStaminaChangedDelegate OnEnemyStaminaChangedDelegate;
	FOnEnemyHealthChangedDelegate OnEnemyHealthChangedDelegate;

	virtual void InitOnSpawn(Protocol::ObjectInfo& Info) override;

	void InitStat();
	void InitStat(float InitHealth);
	void SetHealth(float HealthToSet);
	void AllStop();
};
