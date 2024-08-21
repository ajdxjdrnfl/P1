// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DungeonGameState.generated.h"

/**
 * 
 */
UCLASS()
class P1_API ADungeonGameState : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	void SetPhaseDoorMap();

public:
	TArray<AActor*> PhaseDoors;
	TMap<int32, class APhaseDoor*> PhaseDoorMap;

	void AddAllPhaseDoors();
	void OpenPhaseDoor(int32 PhaseNum);
	class APhaseDoor* GetPhaseDoor(int32 PhaseNum);

};
