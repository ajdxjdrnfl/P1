// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageIndicatorComponentBase.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UDamageIndicatorComponentBase : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	void ActivateDamageIndicatorWidget(class ADamageIndicatorActor* DamageIndicatorActor, float Damage, bool bIsEnemy);

};
