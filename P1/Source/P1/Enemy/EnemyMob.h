// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyMob.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AEnemyMob : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyMob();
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UMobHealthBarWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

};
