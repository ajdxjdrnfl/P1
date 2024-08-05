// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "P1/P1.h"
#include "GameFramework/Actor.h"
#include "BossPillar.generated.h"

UCLASS()
class P1_API ABossPillar : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossPillar();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	class UCapsuleComponent* CapsuleComponent;

public:
	Protocol::ObjectInfo* ObjectInfo;

	UFUNCTION()
	virtual void OnCollisionOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
