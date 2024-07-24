// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatWidgetBase.h"
#include "EnemyStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UEnemyStatWidget : public UStatWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UProgressBar* HealthBar;

	UPROPERTY()
	class UEnemyStatComponent* EnemyStatComponent;

public:
	void SetHealthBar();
	void SetEnemyStat(class UEnemyStatComponent* StatComponent);
};
