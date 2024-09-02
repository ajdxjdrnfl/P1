// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CinemaWidget.generated.h"

/**
 * 
 */
UCLASS()
class P1_API UCinemaWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidgetAnim), meta = (AllowPrivateAccess = "true"), Transient)
	class UWidgetAnimation* CinemaStartAnimation;

	UPROPERTY(meta = (BindWidgetAnim), meta = (AllowPrivateAccess = "true"), Transient)
	class UWidgetAnimation* CinemaEndAnimation;

	FWidgetAnimationDynamicEvent OnCinemaAnimationEndDelegate;

public:
	void PlayCinemaStartAnimation();

	void PlayCinemaEndAnimation();

	UFUNCTION()
	void OnCinemaAnimationEnd();
};
