// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/StatComponent/StatComponentBase.h"
#include "EnemyStatComponent.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UEnemyStatComponent : public UStatComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void TakeDamage() override;
};
