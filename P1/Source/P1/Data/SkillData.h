// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillData.generated.h"

/*UENUM(BlueprintType)
enum class EPawnState : uint8
{
	ELiving UMETA(Displayname = "Living"),
	EDead UMETA(Displayname = "Dead"),
	EMAX UMETA(Displayname = "MAX"),
};

USTRUCT(BlueprintType)
struct FDamageInfo
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Damage;
};*/


USTRUCT(BlueprintType)
struct FSkillInfo : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	// TODO:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SkillType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CooldownTime;
};

USTRUCT(BlueprintType)
struct FSkillsByClass : public FTableRowBase
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkillInfo> SkillInfos;
};
