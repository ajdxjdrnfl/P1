// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillTargetingBase.generated.h"

UCLASS()
class P1_API ASkillTargetingBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillTargetingBase();

	virtual void BeginPlay() override;
	virtual void Init(class ASkillInstanceBase* _SkillInstance);

protected:
	UPROPERTY()
	class AP1Creature* OwnerCreature;
	UPROPERTY()
	class AP1PlayerController* OwnerController;
	UPROPERTY()
	class ASkillInstanceBase* SkillInstance;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* ParticleSystemComponent;

};
