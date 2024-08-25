// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DungeonManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UDungeonManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	void SetPhaseDoorMap();

public:
	UFUNCTION(BlueprintCallable)
	void Init();

	TArray<AActor*> PhaseDoors;
	TMap<int32, class APhaseDoor*> PhaseDoorMap;

	void AddAllPhaseDoors();
	void OpenPhaseDoor(int32 PhaseNum);
	class APhaseDoor* GetPhaseDoor(int32 PhaseNum);

};
