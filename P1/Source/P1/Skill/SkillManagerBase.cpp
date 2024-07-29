// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillManagerBase.h"
#include "P1/P1Character.h"
#include "P1/Component/SkillComponent/CharacterSkillComponent.h"

// Sets default values
ASkillManagerBase::ASkillManagerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkillManagerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkillManagerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ESkillState ASkillManagerBase::GetSkillState()
{
	return OwnerCharacter->GetSkillComponent()->GetSkillState();
}

void ASkillManagerBase::SetSkillState(ESkillState StateToSet)
{
	OwnerCharacter->GetSkillComponent()->SetSkillState(StateToSet);
}


