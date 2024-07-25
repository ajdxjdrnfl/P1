// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "P1/Data/SkillData.h"
#include "SkillComponentBase.generated.h"

UENUM(BlueprintType)
enum class ESkillState : uint8
{
	Normal UMETA(Displayname = "Normal"),
	Hold UMETA(Displayname = "Hold"),
	Casting UMETA(Displayname = "Casting"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class P1_API USkillComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkillComponentBase();

protected:
	virtual void BeginPlay() override;

private:
	FSkillInfo CurrentSkillInfo;
	class UCastingSkillManager* CastingSkillManager;

public:
	TArray<FSkillInfo> Skills;

	ESkillState SkillState;

	UPROPERTY()
	class UDataTable* SkillDataTable;

	UPROPERTY()
	class UAnimInstance* OwnerAnimInstance;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void SetSkills();
	virtual void UseSkill(uint16 SkillIndex);

	FORCEINLINE ESkillState GetSkillState() const { return SkillState; }
	FORCEINLINE void SetSkillState(ESkillState SkillStateToSet) { SkillState = SkillStateToSet; }
};
