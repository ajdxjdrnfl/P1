// Copyright Epic Games, Inc. All Rights Reserved.

#include "P1GameMode.h"
#include "P1PlayerController.h"
#include "P1Character.h"
#include "UObject/ConstructorHelpers.h"
#include "P1ObjectBase.h"
#include "P1/Skill/SkillInstanceBase.h"
#include "P1/Skill/Warrior/WarriorSkillInstance.h"

AP1GameMode::AP1GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AP1PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}

void AP1GameMode::InitWarriorSkillInstance(TArray<class ASkillInstanceBase*> SkillInstances)
{
	SkillInstances[0] = Cast<ASkillInstanceBase>(NewObject<AWarriorQSkillInstance>());
	SkillInstances[1] = Cast<ASkillInstanceBase>(NewObject<AWarriorWSkillInstance>());
	SkillInstances[2] = Cast<ASkillInstanceBase>(NewObject<AWarriorESkillInstance>());
	SkillInstances[3] = Cast<ASkillInstanceBase>(NewObject<AWarriorRSkillInstance>());
}
