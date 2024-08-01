// Fill out your copyright notice in the Description page of Project Settings.


#include "AIControllerEnemy.h"

void AAIControllerEnemy::MoveToTargetLocation(FVector TargetLoc)
{
	MoveToLocation(TargetLoc);
}

void AAIControllerEnemy::MoveToTargetActor(AActor* TargetActor)
{
	MoveToActor(TargetActor);
}
