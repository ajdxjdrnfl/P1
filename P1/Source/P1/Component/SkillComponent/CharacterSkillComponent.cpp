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
	
	{
		if (OwnerCharacter == nullptr) return;

		Protocol::C_MOVE Pkt;
		Protocol::ObjectInfo* ObjectInfo = Pkt.mutable_info();
		Protocol::ObjectInfo* TargetInfo = Pkt.mutable_targetinfo();
		// TODO: State

		TargetInfo->CopyFrom(*OwnerCharacter->ObjectInfo);
		TargetInfo->set_yaw(OwnerCharacter->GetActorRotation().Yaw);
		TargetInfo->set_state(Protocol::MOVE_STATE_JUMP);

		ObjectInfo->CopyFrom(*OwnerCharacter->ObjectInfo);

		SEND_PACKET(Pkt);
	}
}

void UCharacterSkillComponent::DodgeByServer()
{
	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr) return;

	AnimInstance->Montage_Play(M_Dodge_F);

	USkillManagerSubSystem* SubSystem = OwnerCharacter->GetGameInstance()->GetSubsystem<USkillManagerSubSystem>();

	if (SubSystem == nullptr)
		return;

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
