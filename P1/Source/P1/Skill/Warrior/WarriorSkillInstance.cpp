// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorSkillInstance.h"
#include "P1/P1Character.h"
#include "P1/Skill/SkillActorBase.h"
#include "P1/P1PlayerController.h"
#include "Kismet/KismetMathLibrary.h"

AWarriorQSkillInstance::AWarriorQSkillInstance()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWarriorQSkillInstance::BeginPlay()
{
	Super::BeginPlay();
}

void AWarriorQSkillInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTurning && OwnerCharacter)
	{
		FVector MouseLoc = Cast<AP1PlayerController>(OwnerCharacter->GetController())->GetMouseLocation();
		FVector CharacterLoc = FVector(OwnerCharacter->GetActorLocation().X, OwnerCharacter->GetActorLocation().Y, MouseLoc.Z);
		FVector CharacterToMouseVector = MouseLoc - CharacterLoc;

		FRotator TargetRot = FRotator(CharacterToMouseVector.Rotation());
		OwnerCharacter->SetActorRotation(UKismetMathLibrary::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRot, DeltaTime, 100));
	}
}

/*
* Call On Anim Notify
*/
void AWarriorQSkillInstance::SpawnSkill()
{
	if (bDoOnceActivated) return;
	bDoOnceActivated = true; // set false at skill end

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	FVector Loc = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * 200.f;
	FRotator Rot = OwnerCharacter->GetActorRotation();

	// TODO: SendPacket
	SpawnedSkillActor = Cast<ASkillActorBase>(OwnerCharacter->GetWorld()->SpawnActor(SkillActorClass, &Loc, &Rot, SpawnParams));
	if (SpawnedSkillActor == nullptr) return;

	SpawnedSkillActor->InitOnSpawn(OwnerCharacter);
}

void AWarriorQSkillInstance::UseSkill()
{
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || M_Skill == nullptr)
		return;

	bool bIsMontagePlaying = AnimInstance->Montage_IsPlaying(nullptr);
	if (!bIsMontagePlaying)
	{
		AttackIndex = 1;
		AnimInstance->Montage_Play(M_Skill);
	}
	else if (bIsComboTiming)
	{
		AttackIndex++;
		AnimInstance->Montage_JumpToSection(AttackSectionIndex);
		bIsComboTiming = false;
	}
}
