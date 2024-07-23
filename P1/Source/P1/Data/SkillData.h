// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillData.generated.h"

UENUM(BlueprintType)
enum class EPreSkillType : uint8
{
	Immediate UMETA(Displayname = "Normal"),
	Charge UMETA(Displayname = "Dot"),
	Range UMETA(Displayname = "TimeLag"),
};

UENUM(BlueprintType)
enum class ERangePreSkillType : uint8
{
	Circle UMETA(Displayname = "Normal"),
	Charge UMETA(Displayname = "Dot"),
	Range UMETA(Displayname = "TimeLag"),
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Normal UMETA(Displayname = "Normal"),
	Dot UMETA(Displayname = "Dot"),
	TimeLag UMETA(Displayname = "TimeLag"),
};

UENUM(BlueprintType)
enum class ECCType : uint8
{
	Stun UMETA(Displayname = "Stun"),
	Slow UMETA(Displayname = "Slow"),
	Airborne UMETA(Displayname = "Airborne"),
};

USTRUCT(BlueprintType)
struct FGeneralSkillInfo
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ASkillActorBase> SkillActor;
};

USTRUCT(BlueprintType)
struct FSkillActorInfo
{
public:
	GENERATED_USTRUCT_BODY()

	// 타겟팅인지 아닌지
	// 즉발인지 차징인지

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// if HitNum == -1, infinite penetrate
	int32 HitNum = -1;

	// TODO:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCType CCType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageType DamageType;
};

USTRUCT(BlueprintType)
struct FSkillsByClass : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGeneralSkillInfo> SkillInfos;
};
