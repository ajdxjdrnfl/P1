// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillData.generated.h"

enum class EDamageType : uint8;
enum class ECCType : uint8;
enum class ESkillType : uint8;
struct FDamageInfo;
struct FSkillInfo;
struct FSkillsByClass;

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
enum class ECollisionType : uint8
{
	Circle UMETA(Displayname = "Circle"),
	Box UMETA(Displayname = "Box"),
};


UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Normal UMETA(Displayname = "Normal"),
	Hold UMETA(Displayname = "Hold"),
	Casting UMETA(Displayname = "Casting"),
	Charging UMETA(Displayname = "Charging"),
};

UENUM(BlueprintType)
enum class ESkillState : uint8
{
	None UMETA(Displayname = "None"),
	SkillStart UMETA(Displayname = "SkillStart"),
	Skilling UMETA(Displayname = "Skilling"),
	SkillEnd UMETA(Displayname = "SkillEnd"),
};



USTRUCT(BlueprintType)
struct FSkillInfo
{
public:
	GENERATED_USTRUCT_BODY()
	int32 SkillNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillNumLocal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HitNum = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float XScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float YScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* AnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECollisionType CollisionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECCType CCType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageType DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillType SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "SkillType == ESkillType::Hold || SkillType == ESkillType::Casting || SkillType == ESkillType::Charging"))
	float CastingTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "CCType != ECCType::Normal"))
	float CCTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ASkillActorBase> SkillActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* Thumbnail;

};

USTRUCT(BlueprintType)
struct FSkillsByClass : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkillInfo> SkillInfos;
};

