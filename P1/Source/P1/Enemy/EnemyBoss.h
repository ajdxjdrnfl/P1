// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyBoss.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AEnemyBoss : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

private:
	class AAIControllerEnemy* EnemyController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

public:
	void MoveToTargetLocation(FVector TargetLoc);
	void MoveToTargetActor(AActor* TargetActor);
	void UseSkill(int32 SkillIndex);

};
