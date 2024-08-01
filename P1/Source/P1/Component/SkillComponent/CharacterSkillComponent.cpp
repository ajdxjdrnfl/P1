// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkillComponent.h"
#include "Engine/DataTable.h"
#include "P1/P1Character.h"

void UCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SetSkills();
}

void UCharacterSkillComponent::UseSkill(int32 SkillIndex)
{
	Super::UseSkill(SkillIndex);
}

void UCharacterSkillComponent::PlayAnimMontageByServer(bool bIsStop, int SkillIndexLocal, int SectionIndex = -1)
{
	UAnimInstance* AnimInstance = Cast<AP1Character>(GetOwner())->GetMesh()->GetAnimInstance();
	UAnimMontage* AnimMontage = Skills[SkillIndexLocal].AnimMontage;
	if (AnimInstance == nullptr || AnimMontage == nullptr) return;

	if (SectionIndex > 0)
	{
		if (!AnimInstance->Montage_IsPlaying(AnimMontage))
			AnimInstance->Montage_Play(AnimMontage);
		
		FString Str = FString::FromInt(SectionIndex);
		FName SectionIndexName = FName(*Str);
		AnimInstance->Montage_JumpToSection(SectionIndexName);
		return;
	}

	if (!bIsStop)
	{
		AnimInstance->Montage_Play(AnimMontage);
	}
	else
	{
		AnimInstance->Montage_Stop(0.3f, AnimMontage);
	}

}

