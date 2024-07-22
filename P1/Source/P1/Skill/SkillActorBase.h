// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Data/SkillData.h"
#include "SkillActorBase.generated.h"

UCLASS()
class P1_API ASkillActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillActorBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	FSkillInfo SkillInfo;

};
