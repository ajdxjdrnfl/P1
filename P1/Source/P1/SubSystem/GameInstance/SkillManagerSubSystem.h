// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkillManagerSubSystem.generated.h"

/**
 * 
 */
UCLASS()
class P1_API USkillManagerSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	TMap<uint16, bool> SkillCanUseMap;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

};
