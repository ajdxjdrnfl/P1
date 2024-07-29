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

	friend class AP1Character;

private:
	UPROPERTY()
	class AP1Character* OwnerCharacter;

public:
	virtual void UseSkill(uint16 SkillIndex) override;
	
};
