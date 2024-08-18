// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicatorActor.h"
#include "P1/Component/DamageIndicator/DamageIndicatorComponentBase.h"

ADamageIndicatorActor::ADamageIndicatorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DamageIndicatorComponent = CreateDefaultSubobject<UDamageIndicatorComponentBase>(TEXT("DamageIndicatorComponent"));
	DamageIndicatorComponent->SetupAttachment(RootComponent);
}

void ADamageIndicatorActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADamageIndicatorActor::ActivateDamageIndicator(float Damage, bool bIsEnemy)
{
	DamageIndicatorComponent->ActivateDamageIndicatorWidget(this, Damage, bIsEnemy);
}
