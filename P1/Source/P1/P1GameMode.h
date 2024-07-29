// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "P1.h"
#include "P1GameMode.generated.h"

UCLASS(minimalapi)
class AP1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AP1GameMode();

	void InitWarriorSkillInstance(TArray<class ASkillInstanceBase*> SkillInstances);

};



