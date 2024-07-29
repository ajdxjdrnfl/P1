// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P1/Data/SkillData.h"
#include "StatComponentBase.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class P1_API UStatComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatComponentBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void TakeDamage();
		
	float GetCurrentHealth();
	float GetMaxHealth();
	void SetCurrentHealth(float HealthToSet);
	void SetMaxHealth(float HealthToSet);
};
