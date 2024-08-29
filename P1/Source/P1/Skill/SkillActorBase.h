// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Data/SkillData.h"
#include "P1/P1.h"
#include "SkillActorBase.generated.h"

UCLASS()
class P1_API ASkillActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillActorBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	class AP1Creature* OwnerCreature;

public:
	virtual void InitOnSpawn(class AP1Creature* _OwnerCreature);
	virtual void ActivateSkill();
	virtual void SendCollisionPacketToServer(class AP1Creature* Creature);
	virtual void SetCollisionSize(FVector2D SizeToSet) {};
	virtual void BindCollisionDelegate() {};
	Protocol::ObjectInfo* ObjectInfo;
	//Protocol::SkillInfo* SkillInfo;

	TSet<AP1Creature*> DoOnceCCSet;

	UPROPERTY()
	AActor* InstigatorOfSkill;

	UPROPERTY(EditAnywhere)
	FSkillInfo SkillInfo;

	float CurrentLifeTime;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* P_Hit;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* P_Activation;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* P_Hold;

	virtual void SpawnActivationParticleOnLocation(FVector HitLocation);
	virtual void SpawnActivationParticleOnTarget(AActor* TargetActor);
	virtual void SpawnHitParticleOnLocation(FVector HitLocation);
	virtual void SpawnHitParticleOnTarget(AActor* TargetActor);
	virtual class UParticleSystemComponent* SpawnHoldParticleOnLocation(FVector HoldLocation);
	virtual class UParticleSystemComponent* SpawnHoldParticleOnTarget(AActor* TargetActor);
	FVector GetFloorPos(FVector TargetLocation);

	UPROPERTY()
	class ASkillInstanceBase* SkillInstance;

	UFUNCTION()
	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {};
	UFUNCTION()
	virtual void OnCollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {};

};
