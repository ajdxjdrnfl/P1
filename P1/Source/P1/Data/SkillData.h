// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillData.generated.h"

enum class EPreSkillType : uint8;
struct FGeneralSkillInfo;
enum class ERangePreSkillType : uint8;
enum class EDamageType : uint8;
enum class ECCType : uint8;
struct FDamageInfo;
struct FSkillActorInfo;
struct FSkillsByClass;

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
	Buff UMETA(Displayname = "Buff"),
};

UENUM(BlueprintType)
enum class ECCType : uint8
{
	Normal UMETA(Displayname = "Normal"),
	Stun UMETA(Displayname = "Stun"),
	Slow UMETA(Displayname = "Slow"),
	Airborne UMETA(Displayname = "Airborne"),
};

UENUM(BlueprintType)
enum class ESkillShapeType : uint8
{
	Circle UMETA(Displayname = "Circle"),
	Box UMETA(Displayname = "Box"),
};

USTRUCT(BlueprintType)
struct FGeneralSkillInfo
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ASkillActorBase> SkillActor;
}; 

USTRUCT(BlueprintType)
struct FDamageInfo
{
public:
	GENERATED_USTRUCT_BODY()

	// TODO:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCType CCType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageType DamageType;
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
	float StaminaUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillShapeType SkillShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float XScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// if HitNum == -1, infinite penetrate
	int32 HitNum = -1;

	// TODO:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDamageInfo DamageInfo;
};

USTRUCT(BlueprintType)
struct FSkillsByClass : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGeneralSkillInfo> SkillInfos;
};


