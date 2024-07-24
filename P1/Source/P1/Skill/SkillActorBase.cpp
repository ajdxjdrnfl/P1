// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillActorBase.h"
#include "P1/P1.h"

ASkillActorBase::ASkillActorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Info = new Protocol::ObjectInfo();
}

void ASkillActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkillActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkillActorBase::ActivateSkill()
{

}

