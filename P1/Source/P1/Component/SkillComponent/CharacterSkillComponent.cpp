// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterSkillComponent.h"
#include "Engine/DataTable.h"
#include "P1/P1Character.h"
#include "P1/SubSystem/GameInstance/SkillManagerSubSystem.h"

void UCharacterSkillComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCharacterSkillComponent::InitOnSpawn()
{
	Super::InitOnSpawn();

	AP1Character* Character = Cast<AP1Character>(GetOwner());
	if (Character == nullptr) return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr) return;

	AnimInstance->OnMontageEnded.AddUniqueDynamic(this, &UCharacterSkillComponent::OnMontageEnded);
}

void UCharacterSkillComponent::Dodge()
{
	USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>();

	if (SubSystem == nullptr || !SubSystem->bCanMove) 
		return;

	AP1Character* Character = Cast<AP1Character>(GetOwner());

	if (Character == nullptr) 
		return;

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr) 
		return;

	if (AnimInstance->Montage_IsPlaying(nullptr))
		return;

	if (M_Dodge_F == nullptr)
		return;

	AnimInstance->Montage_Play(M_Dodge_F);
	SubSystem->bCanMoveByAnimMontage = false;
}

void UCharacterSkillComponent::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte)
{
	if (AnimMontage == M_Dodge_F)
	{
		if (USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>())
		{
			SubSystem->bCanMoveByAnimMontage = true;
		}
	}
}
