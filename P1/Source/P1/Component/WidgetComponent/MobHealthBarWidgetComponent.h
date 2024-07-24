// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MobHealthBarWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UMobHealthBarWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UMobHealthBarWidgetComponent();

	virtual void BeginPlay() override;

	void SetStatWidgetClass(TSubclassOf<UUserWidget> StatWidget);
};
