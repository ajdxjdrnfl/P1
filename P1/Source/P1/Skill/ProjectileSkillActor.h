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

	FVector MoveDirection;
	float MoveSpeed;
	float MoveDistance;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SkillMeshComponent;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;

public:
	UFUNCTION(BlueprintCallable)
	void MoveProjectile(FVector Direction, float Speed, float Dist, FSkillInfo InfoOfSkill);

	UFUNCTION()
	void OnSphereOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
