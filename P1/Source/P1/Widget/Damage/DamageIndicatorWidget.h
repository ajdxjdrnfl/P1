// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageIndicatorWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UDamageIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	class UTextBlock* DamageIndicatorText;

	UPROPERTY(meta = (BindWidgetAnim), meta = (AllowPrivateAccess = "true"), Transient)
	class UWidgetAnimation* DamageIndicatorAnimation;

	/*UPROPERTY(EditAnywhere, Category = Editable)
	FSlateColor EnemyDamageColor;

	UPROPERTY(EditAnywhere, Category = Editable)
	FSlateColor CharacterDamageColor;*/

	FWidgetAnimationDynamicEvent OnDamageIndicatorAnimationEndDelegate;

	UPROPERTY()
	class ADamageIndicatorActor* DamageIndicatorActor;

public:
	void ActivateDamageIndicatorWidget(class ADamageIndicatorActor* _DamageIndicatorActor, float Damage, bool bIsEnemy);

	UFUNCTION()
	void OnDamageIndicatorAnimationEnd();

	void SetDamageIndicatorTextColor(FLinearColor ColorToSet);
};
