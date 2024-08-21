// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldCursorSkillTargeting.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "P1/P1Creature.h"
#include "P1/P1PlayerController.h"
#include "P1/Skill/SkillInstanceBase.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"

void AWorldCursorSkillTargeting::BeginPlay()
{
	Super::BeginPlay();

	OwnerController->OnRMBClickedDelegate.AddDynamic(this, &AWorldCursorSkillTargeting::OnRMBClicked);
}

void AWorldCursorSkillTargeting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(FVector(OwnerController->GetMouseLocation().X, OwnerController->GetMouseLocation().Y, OwnerCreature->GetActorLocation().Z));
}

void AWorldCursorSkillTargeting::OnRMBClicked()
{
	FVector CursorDirection = OwnerController->GetMouseLocation() - OwnerCreature->GetActorLocation();
	FVector CursorDirection2D = FVector(CursorDirection.X, CursorDirection.Y, 0);

	SkillInstance->UseSkillAfterTargetingWithPos(OwnerController->GetMouseLocation());

	if (USkillManagerSubSystem* SubSystem = OwnerCreature->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
	{
		SubSystem->TargetingSkillLocation = CursorDirection;
	}

	Destroy();
}

void AWorldCursorSkillTargeting::Init(ASkillInstanceBase* _SkillInstance)
{
	Super::Init(_SkillInstance);
}
