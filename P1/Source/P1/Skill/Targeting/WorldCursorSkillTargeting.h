// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/Skill/Targeting/SkillTargetingBase.h"
#include "WorldCursorSkillTargeting.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AWorldCursorSkillTargeting : public ASkillTargetingBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRMBClicked();

	virtual void Init(class ASkillInstanceBase* _SkillInstance) override;

};
