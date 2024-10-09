// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ItemSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class P1_API UItemSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	UPROPERTY(EditAnywhere)
	UDataTable* ItemDataTable;

};
