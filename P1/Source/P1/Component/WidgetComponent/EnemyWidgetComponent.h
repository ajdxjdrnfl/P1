// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetComponentBase.h"
#include "EnemyWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UEnemyWidgetComponent : public UWidgetComponentBase
{
	GENERATED_BODY()

public:
	friend class AEnemyBase;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	class AEnemyBase* OwnerEnemy;

	UPROPERTY(EditAnywhere)
	class UEnemyStatWidget* StatWidget;


public:
	UFUNCTION()
	void OnEnemyHealthChanged();

	void SetEnemyStat(class UEnemyStatComponent* StatComponent);

	FORCEINLINE void SetStatWidget(class UEnemyStatWidget* StatWidgetToSet) { StatWidget = StatWidgetToSet; }

};
