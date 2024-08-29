// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillComponentBase.h"
#include "EnemySkillComponent.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UEnemySkillComponent : public USkillComponentBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	friend class AEnemyBase;

private:
	UPROPERTY()
	class AEnemyBase* OwnerEnemy;

public:
	virtual void InitOnSpawn();
	virtual void UseSkill(int32 SkillIndex) override;
	void UsePillarSkill(FVector2D Location);

	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte) override;
};
