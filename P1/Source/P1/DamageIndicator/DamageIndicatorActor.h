// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageIndicatorActor.generated.h"

UCLASS()
class P1_API ADamageIndicatorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageIndicatorActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UDamageIndicatorComponentBase* DamageIndicatorComponent;

public:
	void ActivateDamageIndicator(float Damage, bool bIsEnemy);

};
