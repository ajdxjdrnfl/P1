// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "P1/Data/SKillData.h"
#include "SkillPositionPredictor.generated.h"

UCLASS()
class P1_API ASkillPositionPredictor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkillPositionPredictor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* PredictorMesh;

	float LifeTime;

public:
	void InitOnSpawn(float _LifeTime);

};
