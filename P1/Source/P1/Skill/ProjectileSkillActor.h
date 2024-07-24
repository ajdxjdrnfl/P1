// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillActorBase.h"
#include "ProjectileSkillActor.generated.h"

/**
 * 
 */
UCLASS()
class P1_API AProjectileSkillActor : public ASkillActorBase
{
	GENERATED_BODY()
	
	AProjectileSkillActor();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	bool bIsActivated;

	UPROPERTY(EditAnywhere)
	FVector MoveDirection;

	UPROPERTY(EditAnywhere)
	float MoveSpeed;

	UPROPERTY(EditAnywhere)
	float MoveDistance;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SkillMeshComponent;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;

public:
	UFUNCTION(BlueprintCallable)
	virtual void ActivateSkill() override;

	void SendCollisionPacketToServer(class AEnemyBase* Enemy);

	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnCollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

};
