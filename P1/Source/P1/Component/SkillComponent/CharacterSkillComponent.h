// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/SkillComponent/SkillComponentBase.h"
#include "CharacterSkillComponent.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UCharacterSkillComponent : public USkillComponentBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void InitOnSpawn() override;

	friend class AP1Character;

private:
	UPROPERTY()
	class AP1Character* OwnerCharacter;

	UPROPERTY(EditAnywhere, Category = Editable)
	class UAnimMontage* M_Dodge_F;

public:
	void Dodge();
	void DodgeByServer();
	
	virtual void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupte) override;
};
