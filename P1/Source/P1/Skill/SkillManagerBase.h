// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Data/SkillData.h"
#include "SkillManagerBase.generated.h"

UCLASS()
class P1_API ASkillManagerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillManagerBase();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	class AP1Character* OwnerCharacter;

	UPROPERTY()
	class ASkillActorBase* SkillActor;

	FSkillInfo SkillInfo;

	UPROPERTY()
	class ASkillInstanceBase* SkillInstance;

public:
	ESkillType GetSkillState();
	void SetSkillState(ESkillType StateToSet);

	virtual void Init(class AP1Character* _OwnerCharacter, class ASkillInstanceBase* _SkillInstance, FSkillInfo _SkillInfo);
	virtual void StartCasting(float CastingTime) {};

	UFUNCTION()
	virtual void OnCastingEnd() {};

};
