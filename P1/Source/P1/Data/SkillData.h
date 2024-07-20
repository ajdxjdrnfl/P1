// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillData.generated.h"

UENUM(BlueprintType)
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
};
