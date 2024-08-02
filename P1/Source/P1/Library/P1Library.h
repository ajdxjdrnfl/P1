// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "P1/Data/SkillData.h"
#include "P1Library.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UP1Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UP1Library();

private:
	UPROPERTY()
	class UStaticMesh* BossSkillRangeMesh;

public:
	static void DrawBossSkillRange(FVector Location, FSkillInfo SkillInfo);

};
