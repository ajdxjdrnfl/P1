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

private:
	UPROPERTY(EditAnywhere)
	class UEnemyStatComponent* StatComponent;

	UPROPERTY(EditAnywhere)
	class UEnemyWidgetComponent* WidgetComponent;

public:
	void InitOnSpawn(float HealthToSet);
	void OnSpawn(float HealthToSet);
	void TakeDamage();
	void SetHealthByDamage(float HealthToSet);

	UFUNCTION()
	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FORCEINLINE class UEnemyStatComponent* GetStatComponent() { return StatComponent; }
	FORCEINLINE class UEnemyWidgetComponent* GetWidgetComponent() { return WidgetComponent; }
};
