// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/P1Creature.h"
#include "P1/Data/SkillData.h"
#include "EnemyBase.generated.h"

UCLASS()
class P1_API AEnemyBase : public AP1Creature
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

	void Init();
	

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UEnemyStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UEnemyWidgetComponent* WidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UEnemySkillComponent* SkillComponent;

public:
	void InitOnSpawn(float HealthToSet);
	void OnSpawn(float HealthToSet);
	void TakeDamage();
	virtual void SetHealthByDamage(float HealthToSet) override;
	virtual void Die() override;
	virtual void MoveByServer(float DeltaTime) override;
	virtual void PlayAnimMontageByServer(Protocol::S_MONTAGE& pkt) override;
	virtual FSkillInfo GetSkillInfoByIndex(int32 SkillIndex) override;


	FORCEINLINE class UEnemyStatComponent* GetStatComponent() { return StatComponent; }
	FORCEINLINE class UEnemyWidgetComponent* GetWidgetComponent() { return WidgetComponent; }
};
