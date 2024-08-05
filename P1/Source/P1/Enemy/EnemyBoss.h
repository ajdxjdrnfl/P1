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
	bool bIsAttackMode;

	UPROPERTY()
	class AAIControllerEnemy* EnemyController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

public:
	void MoveToTargetLocation(FVector TargetLoc);
	void MoveToTargetActor(AActor* TargetActor);
	void UseSkill(int32 SkillIndex);
	void UsePillarSkill(FVector2D Location);
	void SetAttackMode(bool bAttackMode);

	UFUNCTION()
	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
