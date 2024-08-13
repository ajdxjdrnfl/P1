// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnerToCursorSkillTargeting.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "P1/P1Creature.h"
#include "P1/P1PlayerController.h"
#include "P1/Skill/SkillInstanceBase.h"

void AOwnerToCursorSkillTargeting::BeginPlay()
{
	Super::BeginPlay();

	OwnerController->OnRMBClickedDelegate.AddDynamic(this, &AOwnerToCursorSkillTargeting::OnRMBClicked);
	AttachToActor(OwnerCreature, FAttachmentTransformRules::KeepRelativeTransform);
}

void AOwnerToCursorSkillTargeting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CursorDirection = OwnerController->GetMouseLocation() - OwnerCreature->GetActorLocation();
	FVector CursorDirection2D = FVector(CursorDirection.X, CursorDirection.Y, 0);

	SetActorRotation(CursorDirection2D.Rotation());
}

void AOwnerToCursorSkillTargeting::OnRMBClicked()
{
	FVector CursorDirection = OwnerController->GetMouseLocation() - OwnerCreature->GetActorLocation();
	FVector CursorDirection2D = FVector(CursorDirection.X, CursorDirection.Y, 0);

	OwnerCreature->SetActorRotation(CursorDirection2D.Rotation());
	SkillInstance->UseSkillAfterTargeting();
	Destroy();
}

void AOwnerToCursorSkillTargeting::Init(ASkillInstanceBase* _SkillInstance, float Length)
{
	FVector CurrentScale = GetActorScale3D();
	SetActorScale3D(FVector(CurrentScale.X, CurrentScale.Y * Length, CurrentScale.Z));
}
