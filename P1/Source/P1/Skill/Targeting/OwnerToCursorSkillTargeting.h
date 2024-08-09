// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTargetingBase.h"
#include "OwnerToCursorSkillTargeting.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AOwnerToCursorSkillTargeting : public ASkillTargetingBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRMBClicked();

	void Init(class ASkillInstanceBase* _SkillInstance, float Length);

};
