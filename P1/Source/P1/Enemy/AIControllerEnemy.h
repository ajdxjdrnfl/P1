// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerEnemy.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AAIControllerEnemy : public AAIController
{
	GENERATED_BODY()

public:
	void MoveToTargetLocation(FVector TargetLoc);
	void MoveToTargetActor(AActor* TargetActor);
	
};
