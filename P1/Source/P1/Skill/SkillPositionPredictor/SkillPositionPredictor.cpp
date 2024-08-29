// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillPositionPredictor.h"

ASkillPositionPredictor::ASkillPositionPredictor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PredictorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PredictionMesh"));
	PredictorMesh->SetupAttachment(RootComponent);
}

void ASkillPositionPredictor::BeginPlay()
{
	Super::BeginPlay();

	PredictorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASkillPositionPredictor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LifeTime > 0)
	{
		LifeTime -= DeltaTime;
	}
	else
	{
		if (IsValid(this))
		{
			Destroy();
		}
	}
}

void ASkillPositionPredictor::InitOnSpawn(float _LifeTime)
{
	LifeTime = _LifeTime;
}
