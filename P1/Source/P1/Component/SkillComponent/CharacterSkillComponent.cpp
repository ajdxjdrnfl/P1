// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkillComponent.h"
#include "Engine/DataTable.h"
#include "P1/P1Character.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"

void UCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	SetSkills();

	AP1Character* Character = Cast<AP1Character>(GetOwner());
	if (Character == nullptr) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr) return;

	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &UCharacterSkillComponent::OnMontageEnded);
}

void UCharacterSkillComponent::Dodge(float Dot, float Cross)
{
	USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>();
	if (SubSystem == nullptr || !SubSystem->bCanMove) return;

	AP1Character* Character = Cast<AP1Character>(GetOwner());
	if (Character == nullptr) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr) return;

	if (AnimInstance->Montage_IsPlaying(nullptr))
		return;

	if (Dot > -0.5f && Dot < 0.5f && Cross > 0)
	{
		AnimInstance->Montage_Play(M_Dodge_R);
		SubSystem->bCanMoveByAnimMontage = false;
	}
	else if (Dot > -0.5f && Dot < 0.5f && Cross < 0)
	{
		AnimInstance->Montage_Play(M_Dodge_L);
		SubSystem->bCanMoveByAnimMontage = false;
	}
	else if (Dot > 0.5f)
	{
		AnimInstance->Montage_Play(M_Dodge_F);
		SubSystem->bCanMoveByAnimMontage = false;
	}
	else if (Dot < -0.5f)
	{
		AnimInstance->Montage_Play(M_Dodge_B);
		SubSystem->bCanMoveByAnimMontage = false;
	}

}

void UCharacterSkillComponent::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte)
{
	if (AnimMontage == M_Dodge_F || AnimMontage == M_Dodge_B || AnimMontage == M_Dodge_R || AnimMontage == M_Dodge_L)
	{
		if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->bCanMoveByAnimMontage = true;
		}
	}
}
