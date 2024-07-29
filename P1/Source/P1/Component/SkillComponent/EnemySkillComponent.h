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
	virtual void UseSkill(uint16 SkillIndex) override;

};
