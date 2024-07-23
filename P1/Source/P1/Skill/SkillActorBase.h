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
	virtual void Tick(float DeltaTime) override;

private:	
	UPROPERTY(EditAnywhere)
	FSkillActorInfo SkillActorInfo;

public:
	virtual void ActivateSkill();

	UFUNCTION()
	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {};
	UFUNCTION()
	virtual void OnCollisionOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {};

	FORCEINLINE FSkillActorInfo GetSkillInfo() const { return SkillActorInfo; }

};
