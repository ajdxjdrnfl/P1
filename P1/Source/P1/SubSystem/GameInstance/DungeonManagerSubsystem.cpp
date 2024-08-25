// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "P1/Phase/PhaseDoor.h"


void UDungeonManagerSubsystem::Init()
{
	AddAllPhaseDoors();
}

void UDungeonManagerSubsystem::SetPhaseDoorMap()
{
	for (AActor* CurrentActor : PhaseDoors)
	{
		APhaseDoor* PhaseDoor = Cast<APhaseDoor>(CurrentActor);
		if (PhaseDoor == nullptr) continue;

		check(!PhaseDoorMap.Contains(PhaseDoor->GetPhaseID()));

		PhaseDoorMap.Add(PhaseDoor->GetPhaseID(), PhaseDoor);
	}
}

void UDungeonManagerSubsystem::AddAllPhaseDoors()
{
	PhaseDoors.Empty();
	UGameplayStatics::GetAllActorsOfClass(this, APhaseDoor::StaticClass(), PhaseDoors);
	SetPhaseDoorMap();
}

void UDungeonManagerSubsystem ::OpenPhaseDoor(int32 PhaseNum)
{
	if (PhaseDoorMap.Contains(PhaseNum))
	{
		APhaseDoor* PhaseDoor = *PhaseDoorMap.Find(PhaseNum);
		PhaseDoor->ClearPhase();
	}
}

APhaseDoor* UDungeonManagerSubsystem::GetPhaseDoor(int32 PhaseNum)
{
	if (PhaseDoorMap.Contains(PhaseNum))
	{
		return *PhaseDoorMap.Find(PhaseNum);
	}
	return nullptr;
}
