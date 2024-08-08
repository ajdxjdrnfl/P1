// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTargetingBase.h"
#include "Kismet/GameplayStatics.h"
#include "P1/P1Creature.h"
#include "P1/P1PlayerController.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ASkillTargetingBase::ASkillTargetingBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);
}

void ASkillTargetingBase::BeginPlay()
{
	Super::BeginPlay();

	OwnerCreature = Cast<AP1Creature>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	OwnerController = Cast<AP1PlayerController>(OwnerCreature->GetController());
}

void ASkillTargetingBase::Init(ASkillInstanceBase* _SkillInstance)
{
	SkillInstance = _SkillInstance;
}
