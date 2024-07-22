// Copyright Epic Games, Inc. All Rights Reserved.

#include "P1GameMode.h"
#include "P1PlayerController.h"
#include "P1Character.h"
#include "UObject/ConstructorHelpers.h"
#include "P1ObjectBase.h"

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
